/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveThumbnail.h"
#include "ADSRComponent.h"
#include "RompMenu.h"
#include "PulsarUIFeel.h"
#include "DrawableComponent.h"


//==============================================================================
/**
*/
class RomplerAudioProcessorEditor  : public AudioProcessorEditor,
    public Timer, public Button::Listener
{
public:
    RomplerAudioProcessorEditor (RomplerAudioProcessor&);
    ~RomplerAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void timerCallback() override;
    void buttonClicked(Button* b) override;
private:    
    PulsarUIFeel pulsarFeel;

    Rectangle<float> frame;
    std::unique_ptr<WaveThumbnail> mWaveThumbnail;
    std::unique_ptr<ADSRComponent> mADSR;
    std::unique_ptr<RompMenu> rompMenu;
    std::unique_ptr<DrawableComponent> logoComponent;
    std::unique_ptr<TextButton>     stereoFlipButton;
    std::unique_ptr<TextButton>     reverseButton;
    std::unique_ptr<Label>      rompleLabel;


    RomplerAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RomplerAudioProcessorEditor)
};
