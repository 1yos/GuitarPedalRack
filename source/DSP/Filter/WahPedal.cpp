#include "WahPedal.h"

WahPedal::WahPedal() : AudioModule("WahPedal") {}
WahPedal::~WahPedal() {}

void WahPedal::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    reset();
}

void WahPedal::reset()
{
    for (auto& ch : channelState) { ch.x1 = ch.x2 = ch.y1 = ch.y2 = 0.0f; }
}

void WahPedal::setPosition(float position) { positionParam.store(juce::jlimit(0.0f, 1.0f, position)); }
void WahPedal::setResonance(float resonance) { resonanceParam.store(juce::jlimit(0.0f, 1.0f, resonance)); }

void WahPedal::processInternal(AudioBuffer<float>& buffer)
{
    const float position  = positionParam.load();
    const float resonance = resonanceParam.load();

    // Bandpass peak filter (bandpass wah characteristic)
    // Centre frequency sweeps from ~400 Hz to ~2.4 kHz
    const float freq  = 400.0f + position * 2000.0f;
    const float Q     = 2.0f + resonance * 12.0f;
    const float w0    = juce::MathConstants<float>::twoPi * freq / (float)currentSampleRate;
    const float alpha = std::sin(w0) / (2.0f * Q);

    // Bandpass BPF coefficients (peak BPF: H(s) = s/Q / (s^2 + s/Q + 1))
    const float b0 =  alpha;
    const float b1 =  0.0f;
    const float b2 = -alpha;
    const float a0 =  1.0f + alpha;
    const float a1 = -2.0f * std::cos(w0);
    const float a2 =  1.0f - alpha;

    // Normalise
    const float nb0 = b0 / a0;
    const float nb1 = b1 / a0;
    const float nb2 = b2 / a0;
    const float na1 = a1 / a0;
    const float na2 = a2 / a0;

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        if (ch >= (int)channelState.size()) break;
        auto* data = buffer.getWritePointer(ch);
        auto& s    = channelState[(size_t)ch];

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const float x = data[i];
            // Correct Direct Form I:
            // y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
            const float y = nb0 * x + nb1 * s.x1 + nb2 * s.x2
                                     - na1 * s.y1  - na2 * s.y2;
            // Shift state
            s.x2 = s.x1;  s.x1 = x;
            s.y2 = s.y1;  s.y1 = y;
            data[i] = y;
        }
    }
}
