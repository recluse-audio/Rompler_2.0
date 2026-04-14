/*
  ==============================================================================

    WaveThumbnail.cpp
    Created: 4 Apr 2020 5:47:38pm
    Author:  Joshua Hodge

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveThumbnail.h"

//==============================================================================
WaveThumbnail::WaveThumbnail (RomplerAudioProcessor& p) : processor (p)
{
    startTimerHz(60);
    setLookAndFeel(&pulsarFeel);

    rompleLabel = std::make_unique<Label>("");
    rompleLabel->setAlwaysOnTop(true);
    rompleLabel->setFont(40.f);
    addAndMakeVisible(rompleLabel.get());

    dropLabel = std::make_unique<Label>("Drop or Load");
    dropLabel->setAlwaysOnTop(true);
    dropLabel->setFont(40.f);
    addAndMakeVisible(dropLabel.get());
}

WaveThumbnail::~WaveThumbnail()
{
}

void WaveThumbnail::paint (Graphics& g)
{
    auto color1 = Colour().fromRGBA(14, 15, 25, 127);
    auto color2 = Colour().fromRGBA(20, 25, 35, 127);
    auto point = getLocalBounds().getCentre().toFloat();
    auto point2 = getLocalBounds().getBottomLeft().toFloat();


    auto fill = ColourGradient(color2, point, color1, point2, true);
    g.setGradientFill (fill);
    g.fillAll();
    
    auto waveform = processor.getWaveForm();
    
    if (waveform.getNumSamples() > 0)
    {
        dropLabel->setVisible(false);
        rompleLabel->setText(mFileName, juce::NotificationType::dontSendNotification);
        if (waveform.getNumChannels() > 1)
        {
            drawStereo(g, waveform);
        }
        else
        {
            drawMono(g, waveform);
        }

        playHeadPosition = jmap<int>(processor.getSampleCount(), 0, processor.getWaveForm().getNumSamples(), 0, getWidth());

    }
    else
    {
        dropLabel->setFont(40.f);
        dropLabel->setText("Drop or Load", NotificationType::sendNotificationAsync);
        dropLabel->setVisible(true);
    }
}

void WaveThumbnail::drawMono(Graphics& g, AudioBuffer<float>& waveform)
{
    Path p;
    mAudioPoints.clear();

    auto ratio = waveform.getNumSamples() / getWidth();
    auto buffer = waveform.getReadPointer(0);

    //scale audio file to window on x axis
    for (int sample = 0; sample < waveform.getNumSamples(); sample += ratio)
    {
        mAudioPoints.push_back(buffer[sample]);
    }

    g.setColour(Colours::lightgreen);
    p.startNewSubPath(0, getHeight() / 2);

    //scale on y axis
    for (int sample = 0; sample < mAudioPoints.size(); ++sample)
    {
        auto point = jmap<float>(mAudioPoints[sample], -1.0f, 1.0f, getHeight(), 0);
        p.lineTo(sample, point);
    }

    g.strokePath(p, PathStrokeType(0.5f));

   
}

void WaveThumbnail::drawStereo(Graphics& g, AudioBuffer<float>& waveform)
{
    for (int channel = 0; channel < waveform.getNumChannels(); channel++)
    {
        Path wave, ampWave;
        mAudioPoints.clear();

        auto ratio = waveform.getNumSamples() / getWidth();
        auto buffer = waveform.getReadPointer(channel);

        //scale audio file to window on x axis
        for (int sample = 0; sample < waveform.getNumSamples(); sample += ratio)
        {
            mAudioPoints.push_back(buffer[sample]);
        }

        g.setColour(Colours::lightgreen);

        float height = 0.f;

        if (channel == 0)
        {
            height = getHeight() * 0.25f;
        }
        if (channel == 1)
        {
            height = getHeight() * 0.65f;
        }

        wave.startNewSubPath(0, height);
        ampWave.startNewSubPath(0, height);

        float amp = processor.getRMSValue(channel) * 10.f;

        //scale on y axis
        for (int sample = 0; sample < mAudioPoints.size(); ++sample)
        {
            float maxY = height + (getHeight() / 5);
            float minY = height - (getHeight() / 5);
            auto yValue = jmap<float>(mAudioPoints[sample] * 1.f, -1.f, 1.f, maxY, minY);

            /*float phAmp = 1.f;
            if (sample >= playHeadPosition - 20 && sample <= playHeadPosition + 20)
            {
                auto a = playHeadPosition - sample;
                phAmp = 1.f + (a/20.f);
            }*/
            auto s = jlimit(-1.f, 1.f, mAudioPoints[sample] * amp);
            auto yAmp = jmap<float>(s, -1.f, 1.f, maxY, minY);

            wave.lineTo(sample, yValue);
            ampWave.lineTo(sample, yAmp);
        }

        g.strokePath(wave, PathStrokeType(0.5f));

        g.setColour(juce::Colours::aquamarine.brighter());
        g.strokePath(ampWave, PathStrokeType(0.75));
    }

}

void WaveThumbnail::resized()
{
    rompleLabel->setBoundsRelative(0.7f, 0.f, 0.3f, 0.2f);
    dropLabel->setBoundsRelative(0.3f, 0.2f, 0.4f, 0.4f);

}

bool WaveThumbnail::isInterestedInFileDrag (const StringArray& files)
{
    for (auto file : files)
    {
        if (file.contains (".wav") || file.contains (".mp3") || file.contains (".aif"))
        {
            return true;
        }
    }
    
    return false;
}

void WaveThumbnail::filesDropped (const StringArray& files, int x, int y)
{
    for (auto file : files)
    {
        if (isInterestedInFileDrag (file))
        {
            auto myFile = std::make_unique<File>(file);
            mFileName = myFile->getFileNameWithoutExtension();
            processor.setRompleName(mFileName);
            processor.loadFile (file);
        }
    }
    
    repaint();
}

void WaveThumbnail::timerCallback()
{
    repaint();
}
