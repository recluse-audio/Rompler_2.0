/*
  ==============================================================================

    WaveThumbnail.h
    Created: 4 Apr 2020 5:47:38pm
    Author:  Joshua Hodge

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PulsarUIFeel.h"

//==============================================================================
/*
*/
class WaveThumbnail : public Component,
                      public FileDragAndDropTarget, Timer
                        
{
public:
    WaveThumbnail (RomplerAudioProcessor& p);
    ~WaveThumbnail();

    void paint (Graphics&) override;
    void drawMono(Graphics&, AudioBuffer<float>&);
    void drawStereo(Graphics&, AudioBuffer<float>&);
    void resized() override;
    
    bool isInterestedInFileDrag (const StringArray& files) override;
    void filesDropped (const StringArray& files, int x, int y) override;

    void timerCallback() override;
private:
    PulsarUIFeel pulsarFeel;
    std::vector<float> mAudioPoints;
    bool mShouldBePainting { false };
    int playHeadPosition = -1;
    
    String mFileName { "" };
    
    std::unique_ptr<Label> rompleLabel;
    std::unique_ptr<Label> dropLabel;
    RomplerAudioProcessor& processor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveThumbnail)
};
