cmake_minimum_required(VERSION 3.22)
project(Maple VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        9.0.5
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(Maple
    COMPANY_NAME "Hanging Puppet Audio"
    VST3_CATEGORIES Fx Distortion
    BUNDLE_ID "com.hangingpuppetaudio.maple"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE HpAu
    PLUGIN_CODE Mple
    FORMATS VST3
    PRODUCT_NAME "Maple"
)

juce_add_binary_data(PluginAssets
    SOURCES
        Resources/EBGaramond.ttf
        Resources/Inter.ttf
)

target_sources(Maple PRIVATE
    PluginProcessor.h
    PluginProcessor.cpp
    PluginEditor.h
    PluginEditor.cpp
)

target_compile_definitions(Maple PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(Maple PRIVATE
    PluginAssets
    juce::juce_audio_utils
    juce::juce_dsp
PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)