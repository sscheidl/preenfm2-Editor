cmake_minimum_required(VERSION 3.22)

project(PreenfmEditor
    VERSION 4.0.1
    DESCRIPTION "MIDI editor for the preenfm2 and preenfm3 synthesizers"
    LANGUAGES C CXX)

include(FetchContent)

set(JUCE_TAG "9.0.0" CACHE STRING "JUCE release tag used for reproducible builds")
set(JUCE_SOURCE_DIR "" CACHE PATH "Optional path to an existing JUCE checkout")

if(JUCE_SOURCE_DIR)
    add_subdirectory("${JUCE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/JUCE")
else()
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG "${JUCE_TAG}"
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE)
    FetchContent_MakeAvailable(JUCE)
endif()

juce_add_plugin(PreenfmEditor
    VERSION ${PROJECT_VERSION}
    COMPANY_NAME "Xavier Hosxe"
    COMPANY_WEBSITE "https://github.com/Ixox/preenfm2Controller"
    BUNDLE_ID "com.ixox.preenfmEditor"
    PLUGIN_MANUFACTURER_CODE Ixox
    PLUGIN_CODE IxPF
    FORMATS VST3 Standalone
    PRODUCT_NAME "PreenFM+"
    DESCRIPTION "PreenFM+ MIDI editor"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES Instrument)

juce_generate_juce_header(PreenfmEditor)

target_sources(PreenfmEditor PRIVATE
    Plugin/Source/ListProperty.cpp
    Plugin/Source/ListProperty.h
    Plugin/Source/MidifiedFloatParameter.cpp
    Plugin/Source/MidifiedFloatParameter.h
    Plugin/Source/Pfm2MidiDevice.cpp
    Plugin/Source/Pfm2MidiDevice.h
    Plugin/Source/PluginEditor.cpp
    Plugin/Source/PluginEditor.h
    Plugin/Source/PluginProcessor.cpp
    Plugin/Source/PluginProcessor.h
    Plugin/Source/PreenNrpn.h
    Plugin/Source/UI/Enveloppe.cpp
    Plugin/Source/UI/Enveloppe.h
    Plugin/Source/UI/EnveloppeAbstract.cpp
    Plugin/Source/UI/EnveloppeAbstract.h
    Plugin/Source/UI/EnveloppeFree1.cpp
    Plugin/Source/UI/EnveloppeFree1.h
    Plugin/Source/UI/EnveloppeFree2.cpp
    Plugin/Source/UI/EnveloppeFree2.h
    Plugin/Source/UI/EnveloppeListener.h
    Plugin/Source/UI/MainTabs.cpp
    Plugin/Source/UI/MainTabs.h
    Plugin/Source/UI/PanelArpAndFilter.cpp
    Plugin/Source/UI/PanelArpAndFilter.h
    Plugin/Source/UI/PanelEngine.cpp
    Plugin/Source/UI/PanelEngine.h
    Plugin/Source/UI/PanelModulation.cpp
    Plugin/Source/UI/PanelModulation.h
    Plugin/Source/UI/PanelOfComponents.h
    Plugin/Source/UI/PreenLookAndFeel.cpp
    Plugin/Source/UI/PreenLookAndFeel.h
    Plugin/Source/UI/SliderPfm2.h
    Plugin/Source/UI/StepSequencer.cpp
    Plugin/Source/UI/StepSequencer.h
    Plugin/Source/UI/StepSequencerListener.h)

target_include_directories(PreenfmEditor PRIVATE
    Plugin/Source
    Plugin/Source/UI)

target_compile_features(PreenfmEditor PRIVATE cxx_std_17)

target_compile_definitions(PreenfmEditor PUBLIC
    JUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING=1
    JUCE_MODAL_LOOPS_PERMITTED=1
    JUCE_REPORT_APP_USAGE=0
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_VST3_COMPONENT_CLASS="49545356507870467265656E666D2065")

target_link_libraries(PreenfmEditor PRIVATE
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_cryptography
    juce::juce_gui_extra
    juce::juce_opengl
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)
