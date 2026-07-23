cmake_minimum_required(VERSION 3.22)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "")

project(GuitarPedalRack VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(../ParametricEQ/JUCE ${CMAKE_BINARY_DIR}/JUCE)

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME "Aether Audio"
    PRODUCT_NAME "Aether Guitar Pedal Rack"
    DESCRIPTION "Professional boutique guitar pedalboard plugin"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE "AeAu"
    PLUGIN_CODE "AGPr"
    FORMATS VST3 Standalone
    VST3_CATEGORIES "Fx" "Guitar" "Distortion"
)

target_sources(${PROJECT_NAME} PRIVATE
    source/PluginProcessor.cpp
    source/PluginProcessor.h
    source/JuceHeader.h

    # DSP Core
    source/DSP/AudioModule.h
    source/DSP/SignalChain.cpp
    source/DSP/SignalChain.h
    source/DSP/SmartSignalChain.cpp
    source/DSP/SmartSignalChain.h
    source/DSP/EffectLibrary.cpp
    source/DSP/EffectLibrary.h
    source/DSP/SIMDProcessor.h

    # Dynamics
    source/DSP/Dynamics/NoiseGate.cpp
    source/DSP/Dynamics/NoiseGate.h
    source/DSP/Dynamics/Compressor.cpp
    source/DSP/Dynamics/Compressor.h
    source/DSP/Dynamics/FETCompressor.cpp
    source/DSP/Dynamics/FETCompressor.h
    source/DSP/Dynamics/VCACompressor.cpp
    source/DSP/Dynamics/VCACompressor.h
    source/DSP/Dynamics/OpticalCompressor.cpp
    source/DSP/Dynamics/OpticalCompressor.h
    source/DSP/Dynamics/TubeCompressor.cpp
    source/DSP/Dynamics/TubeCompressor.h
    source/DSP/Dynamics/Limiter.cpp
    source/DSP/Dynamics/Limiter.h
    source/DSP/Dynamics/Expander.cpp
    source/DSP/Dynamics/Expander.h
    source/DSP/Dynamics/MultibandCompressor.cpp
    source/DSP/Dynamics/MultibandCompressor.h
    source/DSP/Dynamics/DeEsser.cpp
    source/DSP/Dynamics/DeEsser.h

    # Drive
    source/DSP/Drive/TubeOverdrive.cpp
    source/DSP/Drive/TubeOverdrive.h
    source/DSP/Drive/Distortion.cpp
    source/DSP/Drive/Distortion.h
    source/DSP/Drive/TubeScreamer808.cpp
    source/DSP/Drive/TubeScreamer808.h
    source/DSP/Drive/TubeScreamerTS9.cpp
    source/DSP/Drive/TubeScreamerTS9.h
    source/DSP/Drive/TubeScreamerMini.cpp
    source/DSP/Drive/TubeScreamerMini.h
    source/DSP/Drive/KlonCentaur.cpp
    source/DSP/Drive/KlonCentaur.h
    source/DSP/Drive/ProCoRAT.cpp
    source/DSP/Drive/ProCoRAT.h
    source/DSP/Drive/ProCoRAT2.cpp
    source/DSP/Drive/ProCoRAT2.h
    source/DSP/Drive/TurboRAT.cpp
    source/DSP/Drive/TurboRAT.h
    source/DSP/Drive/BossDS1.cpp
    source/DSP/Drive/BossDS1.h
    source/DSP/Drive/BossSD1.cpp
    source/DSP/Drive/BossSD1.h
    source/DSP/Drive/BigMuffPi.cpp
    source/DSP/Drive/BigMuffPi.h
    source/DSP/Drive/BigMuffRussian.cpp
    source/DSP/Drive/BigMuffRussian.h
    source/DSP/Drive/FuzzFace.cpp
    source/DSP/Drive/FuzzFace.h
    source/DSP/Drive/FuzzFaceSilicon.cpp
    source/DSP/Drive/FuzzFaceSilicon.h
    source/DSP/Drive/BluesDriver.cpp
    source/DSP/Drive/BluesDriver.h
    source/DSP/Drive/MetalZone.cpp
    source/DSP/Drive/MetalZone.h
    source/DSP/Drive/TimmyOverdrive.cpp
    source/DSP/Drive/TimmyOverdrive.h

    # Amp & Cabinet
    source/DSP/Amp/AmpSimulator.cpp
    source/DSP/Amp/AmpSimulator.h
    source/DSP/Cabinet/CabinetIR.cpp
    source/DSP/Cabinet/CabinetIR.h

    # Modulation
    source/DSP/Modulation/Chorus.cpp
    source/DSP/Modulation/Chorus.h
    source/DSP/Modulation/ChorusEnsemble.cpp
    source/DSP/Modulation/ChorusEnsemble.h
    source/DSP/Modulation/ChorusDimension.cpp
    source/DSP/Modulation/ChorusDimension.h
    source/DSP/Modulation/BossCE1.cpp
    source/DSP/Modulation/BossCE1.h
    source/DSP/Modulation/BossCE2.cpp
    source/DSP/Modulation/BossCE2.h
    source/DSP/Modulation/Phaser90.cpp
    source/DSP/Modulation/Phaser90.h
    source/DSP/Modulation/Phaser4Stage.cpp
    source/DSP/Modulation/Phaser4Stage.h
    source/DSP/Modulation/Phaser6Stage.cpp
    source/DSP/Modulation/Phaser6Stage.h
    source/DSP/Modulation/FlangerBasic.cpp
    source/DSP/Modulation/FlangerBasic.h
    source/DSP/Modulation/FlangerElectric.cpp
    source/DSP/Modulation/FlangerElectric.h
    source/DSP/Modulation/TremoloOptical.cpp
    source/DSP/Modulation/TremoloOptical.h
    source/DSP/Modulation/TremoloHarmonic.cpp
    source/DSP/Modulation/TremoloHarmonic.h
    source/DSP/Modulation/VibratoClassic.cpp
    source/DSP/Modulation/VibratoClassic.h
    source/DSP/Modulation/VibratoVintage.cpp
    source/DSP/Modulation/VibratoVintage.h
    source/DSP/Modulation/RingModulator.cpp
    source/DSP/Modulation/RingModulator.h
    source/DSP/Modulation/RingModAnalog.cpp
    source/DSP/Modulation/RingModAnalog.h
    source/DSP/Modulation/RotarySpeaker.cpp
    source/DSP/Modulation/RotarySpeaker.h

    # Time FX - Delay
    source/DSP/TimeFX/Delay.cpp
    source/DSP/TimeFX/Delay.h
    source/DSP/TimeFX/AnalogDelay.cpp
    source/DSP/TimeFX/AnalogDelay.h
    source/DSP/TimeFX/TapeDelay.cpp
    source/DSP/TimeFX/TapeDelay.h
    source/DSP/TimeFX/PingPongDelay.cpp
    source/DSP/TimeFX/PingPongDelay.h
    source/DSP/TimeFX/MultiTapDelay.cpp
    source/DSP/TimeFX/MultiTapDelay.h
    source/DSP/TimeFX/ReverseDelay.cpp
    source/DSP/TimeFX/ReverseDelay.h
    source/DSP/TimeFX/SlapbackDelay.cpp
    source/DSP/TimeFX/SlapbackDelay.h
    source/DSP/TimeFX/DualDelay.cpp
    source/DSP/TimeFX/DualDelay.h
    source/DSP/TimeFX/DottedDelay.cpp
    source/DSP/TimeFX/DottedDelay.h
    source/DSP/TimeFX/RhythmicDelay.cpp
    source/DSP/TimeFX/RhythmicDelay.h
    source/DSP/TimeFX/MemoryMan.cpp
    source/DSP/TimeFX/MemoryMan.h
    source/DSP/TimeFX/Echoplex.cpp
    source/DSP/TimeFX/Echoplex.h
    source/DSP/TimeFX/SpaceEcho.cpp
    source/DSP/TimeFX/SpaceEcho.h
    source/DSP/TimeFX/BossDD3.cpp
    source/DSP/TimeFX/BossDD3.h

    # Time FX - Reverb
    source/DSP/TimeFX/Reverb.cpp
    source/DSP/TimeFX/Reverb.h
    source/DSP/TimeFX/PlateReverb.cpp
    source/DSP/TimeFX/PlateReverb.h
    source/DSP/TimeFX/SpringReverb.cpp
    source/DSP/TimeFX/SpringReverb.h
    source/DSP/TimeFX/HallReverb.cpp
    source/DSP/TimeFX/HallReverb.h
    source/DSP/TimeFX/RoomReverb.cpp
    source/DSP/TimeFX/RoomReverb.h
    source/DSP/TimeFX/ChamberReverb.cpp
    source/DSP/TimeFX/ChamberReverb.h
    source/DSP/TimeFX/ShimmerReverb.cpp
    source/DSP/TimeFX/ShimmerReverb.h
    source/DSP/TimeFX/ReverseReverb.cpp
    source/DSP/TimeFX/ReverseReverb.h
    source/DSP/TimeFX/GatedReverb.cpp
    source/DSP/TimeFX/GatedReverb.h
    source/DSP/TimeFX/ModulatedReverb.cpp
    source/DSP/TimeFX/ModulatedReverb.h

    # Utility / EQ
    source/DSP/Utility/EQ.cpp
    source/DSP/Utility/EQ.h
    source/DSP/Utility/GraphicEQ10Band.cpp
    source/DSP/Utility/GraphicEQ10Band.h
    source/DSP/Utility/GraphicEQ31Band.cpp
    source/DSP/Utility/GraphicEQ31Band.h
    source/DSP/Utility/ShelvingEQ.cpp
    source/DSP/Utility/ShelvingEQ.h
    source/DSP/Utility/HighPassEQ.cpp
    source/DSP/Utility/HighPassEQ.h
    source/DSP/Utility/LowPassEQ.cpp
    source/DSP/Utility/LowPassEQ.h
    source/DSP/Utility/ToneStackFender.cpp
    source/DSP/Utility/ToneStackFender.h
    source/DSP/Utility/ToneStackMarshall.cpp
    source/DSP/Utility/ToneStackMarshall.h

    # Filter / Wah
    source/DSP/Filter/AutoWah.cpp
    source/DSP/Filter/AutoWah.h
    source/DSP/Filter/EnvelopeFilter.cpp
    source/DSP/Filter/EnvelopeFilter.h
    source/DSP/Filter/WahPedal.cpp
    source/DSP/Filter/WahPedal.h
    source/DSP/Filter/TalkBox.cpp
    source/DSP/Filter/TalkBox.h
    source/DSP/Filter/LowPassFilter.cpp
    source/DSP/Filter/LowPassFilter.h
    source/DSP/Filter/HighPassFilter.cpp
    source/DSP/Filter/HighPassFilter.h
    source/DSP/Filter/BandPassFilter.cpp
    source/DSP/Filter/BandPassFilter.h
    source/DSP/Filter/NotchFilter.cpp
    source/DSP/Filter/NotchFilter.h
    source/DSP/Filter/FormantFilter.cpp
    source/DSP/Filter/FormantFilter.h
    source/DSP/Filter/StateVariableFilter.cpp
    source/DSP/Filter/StateVariableFilter.h

    # Pitch
    source/DSP/Pitch/PitchShifter.cpp
    source/DSP/Pitch/PitchShifter.h
    source/DSP/Pitch/Harmonizer.cpp
    source/DSP/Pitch/Harmonizer.h
    source/DSP/Pitch/OctaverUp.cpp
    source/DSP/Pitch/OctaverUp.h
    source/DSP/Pitch/OctaverDown.cpp
    source/DSP/Pitch/OctaverDown.h
    source/DSP/Pitch/WhammyPedal.cpp
    source/DSP/Pitch/WhammyPedal.h
    source/DSP/Pitch/DetuneChorus.cpp
    source/DSP/Pitch/DetuneChorus.h
    source/DSP/Pitch/PitchCorrection.cpp
    source/DSP/Pitch/PitchCorrection.h
    source/DSP/Pitch/FormantShifter.cpp
    source/DSP/Pitch/FormantShifter.h
    source/DSP/Pitch/PitchVibrato.cpp
    source/DSP/Pitch/PitchVibrato.h
    source/DSP/Pitch/IntelligentHarmony.cpp
    source/DSP/Pitch/IntelligentHarmony.h

    # State
    source/State/PresetManager.cpp
    source/State/PresetManager.h
    source/State/FactoryPresets.cpp
    source/State/PresetFormat.h

    # UI
    source/UI/EffectBrowser.cpp
    source/UI/EffectBrowser.h
    source/UI/ParameterEditorPanel.cpp
    source/UI/ParameterEditorPanel.h
    source/UI/PresetBrowser.cpp
    source/UI/PresetBrowser.h
    source/UI/ModernPluginEditor.cpp
    source/UI/ModernPluginEditor.h
    source/UI/PedalSlot.cpp
    source/UI/PedalSlot.h
    source/UI/PedalBoardView.cpp
    source/UI/PedalBoardView.h
    source/UI/VintageKnob.cpp
    source/UI/VintageKnob.h
    source/UI/Materials.h
    source/UI/PatchCable.h
    source/UI/WornTextures.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/source/DSP
    ${CMAKE_CURRENT_SOURCE_DIR}/source/State
    ${CMAKE_CURRENT_SOURCE_DIR}/source/UI
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /O2)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -O3)
endif()
