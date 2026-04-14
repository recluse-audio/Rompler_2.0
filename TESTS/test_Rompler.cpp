#include <catch2/catch_test_macros.hpp>
#include <JuceHeader.h>
#include "Processor/PluginProcessor.h"

TEST_CASE("Plugin name is correct", "[plugin]")
{
    RomplerAudioProcessor processor;
    REQUIRE(processor.getName() == juce::String("Rompler_2.0"));
}

TEST_CASE("Plugin reports correct MIDI capabilities", "[plugin]")
{
    RomplerAudioProcessor processor;
    REQUIRE(processor.acceptsMidi() == true);
    REQUIRE(processor.producesMidi() == false);
    REQUIRE(processor.isMidiEffect() == false);
}

TEST_CASE("Plugin has editor", "[plugin]")
{
    RomplerAudioProcessor processor;
    REQUIRE(processor.hasEditor() == true);
}
