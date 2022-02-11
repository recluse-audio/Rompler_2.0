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
#include "PresetMenu.h"

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

    void textInputCallback(StringRef textEntered);
private:    
    PulsarUIFeel pulsarFeel;

    Rectangle<float> frame;
    Rectangle<float> bigFrame;
    std::unique_ptr<WaveThumbnail> mWaveThumbnail;
    std::unique_ptr<ADSRComponent> mADSR;
    std::unique_ptr<RompMenu> rompMenu;
    std::unique_ptr<DrawableComponent> logoComponent;
    std::unique_ptr<TextButton>     stereoFlipButton;
    std::unique_ptr<TextButton>     reverseButton;

    std::unique_ptr<TextButton>     saveButton;


    std::unique_ptr<PresetMenu>  presetMenu;

    std::unique_ptr<Label>      rompleLabel;

    std::unique_ptr<Label>      kingsLabel;



    RomplerAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RomplerAudioProcessorEditor)
};
