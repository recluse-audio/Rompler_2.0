/*
  ==============================================================================

    ADSRComponent.h
    Created: 4 Apr 2020 7:35:53pm
    Author:  Joshua Hodge

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


//==============================================================================
/*
*/
class ADSRComponent    : public Component
{
public:
    ADSRComponent (RomplerAudioProcessor& p);
    ~ADSRComponent();

    void initSliders();

    void paint (Graphics&) override;
    void resized() override;

    void loadImage();
    void changeColor(XmlElement* xml, StringRef color_hex);

private:
    Slider mAttackSlider, mDecaySlider, mSustainSlider, mReleaseSlider;
    Label mAttackLabel, mDecayLabel, mSustainLabel, mReleaseLabel;
    std::unique_ptr<Drawable> recluseDrawable;

    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mAttackAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mDecayAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mSustainAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mReleaseAttachment;
    
    RomplerAudioProcessor& processor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRComponent)
};
