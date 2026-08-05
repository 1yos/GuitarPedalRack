#include "CabinetIR.h"
#include <cmath>

CabinetIR::CabinetIR() : AudioModule("CabinetIR")
{
    // Generate and cache the default IR buffer.
    // We do NOT call convolution.loadImpulseResponse() here because
    // JUCE's Convolution requires prepare() to be called first.
    // The IR is loaded into the convolution engine inside prepare().
    currentIR = generateDefaultIR();
}

void CabinetIR::prepare(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    AudioModule::prepare(sampleRate, maximumExpectedSamplesPerBlock);
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<uint32_t>(maximumExpectedSamplesPerBlock);
    spec.numChannels = 2;
    
    // Must call prepare() before loadImpulseResponse()
    convolution.prepare(spec);
    
    // Now it is safe to load the cached IR into the convolution engine
    if (currentIR.getNumSamples() > 0)
    {
        AudioBuffer<float> irCopy = currentIR;  // loadImpulseResponse takes by move
        convolution.loadImpulseResponse(std::move(irCopy), sampleRate,
                                       juce::dsp::Convolution::Stereo::yes,
                                       juce::dsp::Convolution::Trim::yes,
                                       juce::dsp::Convolution::Normalise::yes);
    }
    
    reset();
}

void CabinetIR::reset()
{
    convolution.reset();
}

bool CabinetIR::loadIRFromFile(const File& file)
{
    if (!file.existsAsFile())
        return false;
    
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file));
    
    if (reader == nullptr)
        return false;
    
    // Load IR into buffer
    AudioBuffer<float> irBuffer(static_cast<int>(reader->numChannels),
                                static_cast<int>(reader->lengthInSamples));
    reader->read(&irBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
    
    currentIRName = file.getFileNameWithoutExtension();
    currentIR = irBuffer;  // Keep a copy
    
    // Only load into convolution if prepare() has already been called
    if (currentSampleRate > 0.0)
    {
        convolution.loadImpulseResponse(std::move(irBuffer), reader->sampleRate,
                                       juce::dsp::Convolution::Stereo::yes,
                                       juce::dsp::Convolution::Trim::yes,
                                       juce::dsp::Convolution::Normalise::yes);
    }
    
    return true;
}

bool CabinetIR::loadBuiltInIR(const String& irName)
{
    // Generate a type-specific IR based on the cabinet name.
    // Each cabinet type uses different frequency shaping to approximate its character.
    currentIRName = irName;
    
    // Select tone character based on cabinet type
    float lowCut = 80.0f, midBoost = 0.0f, highCut = 5000.0f;
    
    if (irName.containsIgnoreCase("Greenback"))
    {
        lowCut = 100.0f; midBoost = 3.0f; highCut = 4000.0f; // Darker, middy
    }
    else if (irName.containsIgnoreCase("2x12") || irName.containsIgnoreCase("Combo"))
    {
        lowCut = 90.0f; midBoost = 1.5f; highCut = 5500.0f; // Warm combo character
    }
    else if (irName.containsIgnoreCase("1x12") || irName.containsIgnoreCase("Celestion"))
    {
        lowCut = 120.0f; midBoost = 2.0f; highCut = 6500.0f; // Focused, articulate
    }
    else if (irName.containsIgnoreCase("DI"))
    {
        lowCut = 40.0f; midBoost = 0.0f; highCut = 18000.0f; // Flat DI response
    }
    // Default: 4x12 V30 — present, slightly scooped
    else
    {
        lowCut = 80.0f; midBoost = -1.0f; highCut = 5000.0f;
    }
    
    currentIR = generateTypedIR(lowCut, midBoost, highCut);
    
    if (currentSampleRate > 0.0)
    {
        AudioBuffer<float> irCopy = currentIR;
        convolution.loadImpulseResponse(std::move(irCopy), currentSampleRate,
                                       juce::dsp::Convolution::Stereo::yes,
                                       juce::dsp::Convolution::Trim::yes,
                                       juce::dsp::Convolution::Normalise::yes);
    }
    return true;
}

StringArray CabinetIR::getBuiltInIRList() const
{
    StringArray irs;
    irs.add("Default 4x12");
    irs.add("4x12 V30 - SM57 On-Axis");
    irs.add("4x12 V30 - SM57 Off-Axis");
    irs.add("4x12 Greenback - SM57");
    irs.add("2x12 Combo - SM57");
    irs.add("1x12 Celestion - U87");
    irs.add("DI Clean");
    return irs;
}

void CabinetIR::setMicPosition(float position)
{
    micPosition = jlimit(0.0f, 1.0f, position);
}

void CabinetIR::setRoomMix(float mix)
{
    roomMix = jlimit(0.0f, 1.0f, mix);
}

void CabinetIR::setOutputLevel(float levelDb)
{
    outputLevelDb = jlimit(-20.0f, 20.0f, levelDb);
    outputLevel = dbToLinear(outputLevelDb);
}

void CabinetIR::processInternal(AudioBuffer<float>& buffer)
{
    // Check bypass parameter
    auto* bypassParam = parameterPointers["bypass"];
    if (bypassParam && bypassParam->load() > 0.5f)
        return;
    
    // FIX: Read "type" parameter and reload IR if it changed
    if (hasParameter("type"))
    {
        int newType = static_cast<int>(getParameterValue("type"));
        if (newType != lastCabType && currentSampleRate > 0.0)
        {
            lastCabType = newType;
            static const char* irNames[] = {
                "Default 4x12",          // 0 = 4x12 V30
                "4x12 Greenback - SM57", // 1 = 4x12 Greenback
                "2x12 Combo - SM57",     // 2 = 2x12 Vintage
                "1x12 Celestion - U87"   // 3 = 1x12 Classic
            };
            const char* cabName = (newType >= 0 && newType < 4) ? irNames[newType] : irNames[0];
            // loadBuiltInIR regenerates the IR and reloads convolution
            loadBuiltInIR(juce::String(cabName));
        }
    }
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    convolution.process(context);
    
    // 2. Retrieve mic parameters
    float micX = getParameterValue("micX");
    float micY = getParameterValue("micY");
    float distance = getParameterValue("distance");
    float axis = getParameterValue("axis");
    
    // Calculate off-axis factor (micX/Y distance + axis angle contribution)
    float axisFactor = axis / 90.0f;
    float offAxisRad = std::sqrt(micX*micX + micY*micY);
    offAxisRad = jmin(1.0f, offAxisRad + axisFactor * 0.3f);
    
    // Lowpass cutoff frequency (15kHz to 1.5kHz based on axis and distance)
    float lpCutoff = 15000.0f - (offAxisRad * 10000.0f) - (distance * 3000.0f);
    lpCutoff = jlimit(1000.0f, 20000.0f, lpCutoff);
    
    // Map cutoff to one-pole coefficient: coeff = 1 - exp(-2 * pi * f / fs)
    float coeff = 1.0f - std::exp(-2.0f * MathConstants<float>::pi * lpCutoff / static_cast<float>(currentSampleRate));
    coeff = jlimit(0.001f, 1.0f, coeff);
    
    // Delay time in samples (0 to 5 ms, e.g. 0 to 220 samples at 44.1kHz)
    float delaySamples = distance * 220.0f;
    
    // Output volume roll-off based on distance
    float distanceGain = 1.0f - (distance * 0.4f); // drops to 60% volume at max distance
    
    // Process samples
    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();
    
    // Resize delay buffers if needed
    if (delayBufferL.size() < 2048)
    {
        delayBufferL.assign(2048, 0.0f);
        delayBufferR.assign(2048, 0.0f);
    }
    
    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& delayBuf = (ch == 0) ? delayBufferL : delayBufferR;
        auto& lpVal = (ch == 0) ? lpValL : lpValR;
        auto& writeIdx = (ch == 0) ? writeIndexL : writeIndexR;
        
        for (int i = 0; i < numSamples; ++i)
        {
            float inputSample = data[i];
            
            // Write to circular delay buffer
            delayBuf[writeIdx] = inputSample;
            
            // Read from delay buffer with linear interpolation
            float readPtr = (float)writeIdx - delaySamples;
            if (readPtr < 0.0f)
                readPtr += delayBuf.size();
                
            int idx1 = (int)readPtr;
            int idx2 = (idx1 + 1) % delayBuf.size();
            float frac = readPtr - (float)idx1;
            
            float delayedSample = delayBuf[idx1] * (1.0f - frac) + delayBuf[idx2] * frac;
            
            // Apply one-pole lowpass filter
            lpVal += coeff * (delayedSample - lpVal);
            
            // Write back processed sample with distance gain
            data[i] = lpVal * distanceGain;
            
            // Update write index
            writeIdx = (writeIdx + 1) % delayBuf.size();
        }
    }
    
    // Apply overall output level
    buffer.applyGain(outputLevel);
}

void CabinetIR::loadDefaultIR()
{
    // Generate and cache a new default IR
    currentIR = generateDefaultIR();
    currentIRName = "Default 4x12";
    
    // Only load into the convolution engine if prepare() has already been called
    if (currentSampleRate > 0.0)
    {
        AudioBuffer<float> irCopy = currentIR;
        convolution.loadImpulseResponse(std::move(irCopy), currentSampleRate,
                                       juce::dsp::Convolution::Stereo::yes,
                                       juce::dsp::Convolution::Trim::yes,
                                       juce::dsp::Convolution::Normalise::yes);
    }
}

AudioBuffer<float> CabinetIR::generateTypedIR(float lowCutHz, float midBoostDb, float highCutHz)
{
    const int irLength = 1024;
    const double sr = currentSampleRate > 0.0 ? currentSampleRate : 44100.0;
    AudioBuffer<float> ir(2, irLength);
    Random random(42);
    
    for (int channel = 0; channel < 2; ++channel)
    {
        auto* data = ir.getWritePointer(channel);
        float lpZ = 0.0f, hpZ = 0.0f;
        
        // Low-pass coefficient (simulates speaker high-cut)
        float lpCoeff = 1.0f - std::exp(-2.0f * MathConstants<float>::pi * highCutHz / (float)sr);
        // High-pass coefficient (simulates low-cut / room coupling)
        float hpCoeff = 1.0f - std::exp(-2.0f * MathConstants<float>::pi * lowCutHz  / (float)sr);
        // Mid boost/cut gain
        float midGain = std::pow(10.0f, midBoostDb / 20.0f);
        
        for (int i = 0; i < irLength; ++i)
        {
            float decay = std::exp(-5.0f * i / irLength);
            float sample = decay * (std::sin(i * 0.25f) + random.nextFloat() * 0.08f - 0.04f);
            
            // Apply low-pass (speaker cone rolloff)
            lpZ += lpCoeff * (sample - lpZ);
            // Apply high-pass (cabinet/room resonance)
            float hpSample = sample - hpZ;
            hpZ += hpCoeff * (sample - hpZ);
            
            // Blend with mid character
            data[i] = lpZ * 0.7f + hpSample * 0.3f * midGain;
        }
    }
    
    float mag = ir.getMagnitude(0, irLength);
    if (mag > 1e-6f) ir.applyGain(1.0f / mag);
    return ir;
}

AudioBuffer<float> CabinetIR::generateDefaultIR()
{
    // Generate a simple IR that simulates a cabinet frequency response
    // Vary the decay and frequency characteristics based on the selected cabinet type
    
    const int irLength = 1024;  // Short IR
    AudioBuffer<float> ir(2, irLength);
    
    Random random;
    
    // Choose parameters based on cabinet type
    float decayRate = 6.0f;     // exponential decay speed (smaller = larger box / longer resonance)
    float frequency = 0.3f;     // resonant frequency multiplier
    float noiseAmount = 0.1f;   // high-frequency air/resonance noise
    
    if (currentIRName.contains("Greenback"))
    {
        decayRate = 8.0f;       // faster decay, tighter bass
        frequency = 0.45f;      // higher resonant frequency (more mid-focus)
        noiseAmount = 0.08f;
    }
    else if (currentIRName.contains("Combo") || currentIRName.contains("2x12"))
    {
        decayRate = 10.0f;      // faster decay, less cabinet volume resonance
        frequency = 0.6f;       // bright, punchy mid-high resonance
        noiseAmount = 0.05f;
    }
    else if (currentIRName.contains("Classic") || currentIRName.contains("1x12"))
    {
        decayRate = 12.0f;      // extremely fast decay, small box
        frequency = 0.8f;       // very bright, focused peak
        noiseAmount = 0.03f;
    }
    
    for (int channel = 0; channel < 2; ++channel)
    {
        auto* channelData = ir.getWritePointer(channel);
        
        for (int i = 0; i < irLength; ++i)
        {
            // Exponential decay envelope
            float decay = std::exp(-decayRate * i / irLength);
            
            // Add some frequency content (simulates cabinet resonance and microphone pickup)
            float sample = decay * (std::sin(i * frequency) + random.nextFloat() * noiseAmount - noiseAmount * 0.5f);
            
            channelData[i] = sample;
        }
    }
    
    // Normalize — guard against a zero-magnitude IR (division by zero → NaN)
    float magnitude = ir.getMagnitude(0, ir.getNumSamples());
    if (magnitude > 1e-6f)
        ir.applyGain(1.0f / magnitude);
    
    return ir;
}

float CabinetIR::dbToLinear(float db)
{
    return std::pow(10.0f, db / 20.0f);
}

void CabinetIR::saveState(MemoryBlock& destData)
{
    AudioModule::saveState(destData);
    
    MemoryOutputStream stream(destData, true);
    stream.writeString(currentIRName);
    stream.writeFloat(micPosition);
    stream.writeFloat(roomMix);
    stream.writeFloat(outputLevelDb);
}

void CabinetIR::loadState(const void* data, int sizeInBytes)
{
    AudioModule::loadState(data, sizeInBytes);
    
    MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    stream.readFloat(); // wetDryMix
    stream.readBool();  // bypassed
    
    String irName = stream.readString();
    setMicPosition(stream.readFloat());
    setRoomMix(stream.readFloat());
    setOutputLevel(stream.readFloat());
    
    // Try to reload the IR
    loadBuiltInIR(irName);
}
