/*
  ==============================================================================

    RompMenu.h
    Created: 23 Jul 2021 5:40:04pm
    Author:  ryand

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PulsarUIFeel.h"
#include "PluginProcessor.h"
#include "Database.h"
#include "Artist.h"



//==============================================================================
/*
*/
class RompMenu  : public juce::Component, juce::MouseListener, juce::Button::Listener
{
public:
    RompMenu(RomplerAudioProcessor& p);
    ~RompMenu() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& e);
    void buttonClicked(Button* b) override;
    void prepareMenu();

    void loadFile(int selection);

private:
    PopupMenu menu;
    DrawableText text;

    PulsarUIFeel pulsarFeel;

    std::unique_ptr<TextButton> menuButton;
    std::unique_ptr<Label> rompleLabel;

    String fileName;

    Database& database;
    RomplerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RompMenu)
};
