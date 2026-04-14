/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RomplerAudioProcessorEditor::RomplerAudioProcessorEditor (RomplerAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setLookAndFeel(&pulsarFeel);
    mWaveThumbnail = std::make_unique<WaveThumbnail>(p);
    addAndMakeVisible (mWaveThumbnail.get());

    mADSR = std::make_unique<ADSRComponent>(p);
    addAndMakeVisible (mADSR.get());

    rompMenu = std::make_unique<RompMenu>(p);
    addAndMakeVisible(rompMenu.get());

    logoComponent = std::make_unique<DrawableComponent>();
    addAndMakeVisible(logoComponent.get());
    logoComponent->setAlwaysOnTop(true);

    stereoFlipButton = std::make_unique<TextButton>("Flip");
    addAndMakeVisible(stereoFlipButton.get());
    stereoFlipButton->addListener(this);

    reverseButton = std::make_unique<TextButton>("Reverse");
    addAndMakeVisible(reverseButton.get());
    reverseButton->addListener(this);

    rompleLabel = std::make_unique<Label>("Romple Name");
    addAndMakeVisible(rompleLabel.get());
    rompleLabel->setColour(Label::backgroundColourId, Colours::aqua.withAlpha(0.1f));

    frame = Rectangle<float>(getWidth() * 0.025f, getHeight() * 0.1f, getWidth() * 0.95f, getHeight() * 0.875f);

    startTimerHz (60);
    
    setSize (600, 400);
}

RomplerAudioProcessorEditor::~RomplerAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void RomplerAudioProcessorEditor::paint (Graphics& g)
{
    auto color = Colour{ 0.001f, 0.f, 0.0005f, 1.f };
    g.fillAll(color);

    g.setColour(color.brighter());
    g.drawRoundedRectangle(frame, 2.f, 3.f);
}

void RomplerAudioProcessorEditor::resized()
{
    frame = Rectangle<float>(getWidth() * 0.025f, getHeight() * 0.1f, getWidth() * 0.95f, getHeight() * 0.875f);

    rompMenu->setBoundsRelative(0.025f, 0.f, 0.95f, 0.1f);
    logoComponent->setBoundsRelative(0.025f, 0.01f, 0.075f, 0.075f);
    rompleLabel->setBoundsRelative(0.78f, 0.125f, 0.175f, 0.075f);

    mWaveThumbnail->setBounds(frame.reduced(2.f, 0.f).toNearestInt());
    mADSR->setBounds(frame.getX(), frame.getHeight() * 0.825, frame.getWidth(), frame.getHeight() * 0.28f);

    auto buttonX = frame.getX() + frame.getWidth() * 0.01;
    auto buttonY = frame.getY() + frame.getHeight() * 0.925;
    auto buttonWidth = frame.getWidth() * 0.1f; auto buttonHeight = frame.getHeight() * 0.05f;

    stereoFlipButton->setBounds(buttonX, buttonY, buttonWidth, buttonHeight);
    reverseButton->setBounds(buttonX + buttonWidth * 1.1f, buttonY, buttonWidth, buttonHeight);
}

void RomplerAudioProcessorEditor::timerCallback()
{
    rompleLabel->setText(processor.getRompleName(), NotificationType::sendNotificationAsync);
    repaint();
}

void RomplerAudioProcessorEditor::buttonClicked(Button* b)
{
    if (b == stereoFlipButton.get())
    {
        processor.flipStereoRomple();
    }
    if (b == reverseButton.get())
    {
        processor.reverseRomple();
    }
}





