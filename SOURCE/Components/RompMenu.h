/*
  ==============================================================================

    RompMenu.h
    Created: 23 Jul 2021 5:40:04pm
    Author:  ryand

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../LookNFeel/PulsarUIFeel.h"
#include "../Processor/PluginProcessor.h"
#include "../ROMPLE_DATA/Database.h"
#include "../ROMPLE_DATA/Artist.h"



//==============================================================================
/*
*/
class RompMenu  : public juce::Component, public juce::Button::Listener
{
public:
    RompMenu(RomplerAudioProcessor& p);
    ~RompMenu() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& e);
    void buttonClicked(Button* b) override;
    void prepareMenu();



private:
    Database database;
    PopupMenu menu;
    DrawableText text;

    PulsarUIFeel pulsarFeel;

    std::unique_ptr<TextButton> menuButton;
    std::unique_ptr<Label> rompleLabel;

    String fileName;
    RomplerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RompMenu)
};
