cmake_minimum_required(VERSION 3.22)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "")

project(GuitarPedalRack VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE as subdirectory
add_subdirectory(../ParametricEQ/JUCE ${CMAKE_BINARY_DIR}/JUCE)

# Create plugin target
juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME "GuitarFX"
    PRODUCT_NAME "Guitar Pedal Rack"
    DESCRIPTION "Professional modular guitar effects plugin"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE "GFXp"
    PLUGIN_CODE "GPRk"
    FORMATS VST3 AU Standalone
    VST3_CATEGORIES "Fx" "Guitar" "Distortion"
    AU_MAIN_TYPE "kAudioUnitType_Effect"
)

# Source files
target_sources(${PROJECT_NAME} PRIVATE
    source/PluginProcessor.cpp
    source/PluginProcessor.h
    source/PluginEditor.cpp
    source/PluginEditor.h
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
    
    # Drive
    source/DSP/Drive/TubeOverdrive.cpp
    source/DSP/Drive/TubeOverdrive.h
    source/DSP/Drive/Distortion.cpp
    source/DSP/Drive/Distortion.h
    
    # Amp
    source/DSP/Amp/AmpSimulator.cpp
    source/DSP/Amp/AmpSimulator.h
    
    # Cabinet
    source/DSP/Cabinet/CabinetIR.cpp
    source/DSP/Cabinet/CabinetIR.h
    
    # Modulation
    source/DSP/Modulation/Chorus.cpp
    source/DSP/Modulation/Chorus.h
    
    # Time FX
    source/DSP/TimeFX/Delay.cpp
    source/DSP/TimeFX/Delay.h
    source/DSP/TimeFX/Reverb.cpp
    source/DSP/TimeFX/Reverb.h
    
    # Utility
    source/DSP/Utility/EQ.cpp
    source/DSP/Utility/EQ.h
    
    # State Management
    source/State/PresetManager.cpp
    source/State/PresetManager.h
    source/State/FactoryPresets.cpp
    source/State/PresetFormat.h
)

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/source/DSP
    ${CMAKE_CURRENT_SOURCE_DIR}/source/State
    ${CMAKE_CURRENT_SOURCE_DIR}/source/UI
)

# Link JUCE modules
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

# Compiler settings
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Compile definitions
target_compile_definitions(${PROJECT_NAME} PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

# Compiler optimizations
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /O2)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -O3)
endif()
