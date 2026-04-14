#include <catch2/catch_test_macros.hpp>
#include <JuceHeader.h>
#include "Processor/PluginProcessor.h"

TEST_CASE("Parameter default values are in range", "[parameters]")
{
    RomplerAudioProcessor processor;
    auto& params = processor.getParameters();

    REQUIRE(params.size() > 0);

    for (auto* param : params)
    {
        auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param);
        if (rangedParam != nullptr)
        {
            float value = rangedParam->getValue();
            REQUIRE(value >= 0.0f);
            REQUIRE(value <= 1.0f);
        }
    }
}

TEST_CASE("ADSR parameters exist", "[parameters]")
{
    RomplerAudioProcessor processor;
    auto& apvts = processor.getValueTree();

    REQUIRE(apvts.getParameter("ATTACK") != nullptr);
    REQUIRE(apvts.getParameter("DECAY") != nullptr);
    REQUIRE(apvts.getParameter("SUSTAIN") != nullptr);
    REQUIRE(apvts.getParameter("RELEASE") != nullptr);
}
