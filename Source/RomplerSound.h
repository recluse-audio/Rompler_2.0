/*
  ==============================================================================

    RomplerSound.h
    Created: 10 Nov 2021 5:11:05am
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class RomplerSound : public SynthesiserSound
{
public:
    RomplerSound(const String& soundName, AudioFormatReader& source,
        const BigInteger& notes, int midiNoteForNormalPitch,
        double attackTimeSecs, double releaseTimeSecs, double maxSampleLengthSeconds)
        : name(soundName), sourceSampleRate(source.sampleRate), midiNotes(notes), midiRootNote(midiNoteForNormalPitch)
    {
        if (sourceSampleRate > 0 && source.lengthInSamples > 0)
        {
            length = jmin((int)source.lengthInSamples,
                (int)(maxSampleLengthSeconds * sourceSampleRate));

            data.reset(new AudioBuffer<float>(jmin(2, (int)source.numChannels), length + 4));

            source.read(data.get(), 0, length + 4, 0, true, true);

            params.attack = static_cast<float> (attackTimeSecs);
            params.release = static_cast<float> (releaseTimeSecs);
        }
    }

    ~RomplerSound() override {}

    const String& getName() const noexcept 
    { 
        return name; 
    }

    AudioBuffer<float>* getAudioData() const noexcept 
    { 
        return data.get(); 
    }

    void setEnvelopeParameters(ADSR::Parameters parametersToUse) 
    { 
        params = parametersToUse; 
    }

    bool appliesToNote(int midiNoteNumber)
    {
        return midiNotes[midiNoteNumber];
    }

    bool appliesToChannel(int /*midiChannel*/)
    {
        return true;
    }

private:
    friend class RomplerVoice;

    String name;
    std::unique_ptr<AudioBuffer<float>> data;
    double sourceSampleRate;
    BigInteger midiNotes;
    int length = 0, midiRootNote = 0;

    ADSR::Parameters params;

    JUCE_LEAK_DETECTOR(RomplerSound)
};

class JUCE_API  RomplerVoice : public SynthesiserVoice
{
public:
    //==============================================================================
    /** Creates a RomplerVoice. */
    RomplerVoice() {}

    /** Destructor. */
    ~RomplerVoice() override {}

    //==============================================================================
    bool canPlaySound(SynthesiserSound* sound) override
    {
        return dynamic_cast<const RomplerSound*> (sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity, SynthesiserSound* s, int pitchWheel) override
    {
        if (auto* sound = dynamic_cast<const RomplerSound*> (s))
        {
            pitchRatio = std::pow(2.0, (midiNoteNumber - sound->midiRootNote) / 12.0)
                * sound->sourceSampleRate / getSampleRate();

            sourceSamplePosition = 0.0;
            lgain = velocity;
            rgain = velocity;

            adsr.setSampleRate(sound->sourceSampleRate);
            adsr.setParameters(sound->params);

            adsr.noteOn();
        }
        else
        {
            jassertfalse; // this object can only play RomplerSounds!
        }
    }
    void stopNote(float velocity, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            adsr.noteOff();
        }
        else
        {
            clearCurrentNote();
            adsr.reset();
        }
    }

    void pitchWheelMoved(int newValue) override {}
    void controllerMoved(int controllerNumber, int newValue) override {}

    void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (auto* playingSound = static_cast<RomplerSound*> (getCurrentlyPlayingSound().get()))
        {
            auto& data = *playingSound->data;
            const float* const inL = data.getReadPointer(0);
            const float* const inR = data.getNumChannels() > 1 ? data.getReadPointer(1) : nullptr;

            float* outL = outputBuffer.getWritePointer(0, startSample);
            float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer(1, startSample) : nullptr;


            while (--numSamples >= 0)
            {
                downSampleIncrement++;

                if (downSampleIncrement >= downSampleRatio)
                {
                    auto pos = (int)sourceSamplePosition;
                    auto alpha = (float)(sourceSamplePosition - pos);
                    auto invAlpha = 1.0f - alpha;
                }
                
                auto pos = (int)sourceSamplePosition;
                auto alpha = (float)(sourceSamplePosition - pos);
                auto invAlpha = 1.0f - alpha;

                // just using a very simple linear interpolation here..
                float l = (inL[pos] * invAlpha + inL[pos + 1] * alpha);
                float r = (inR != nullptr) ? (inR[pos] * invAlpha + inR[pos + 1] * alpha) : l;

                auto envelopeValue = adsr.getNextSample();

                l *= lgain * envelopeValue;
                r *= rgain * envelopeValue;

                if (outR != nullptr)
                {
                    *outL++ += l;
                    *outR++ += r;
                }
                else
                {
                    *outL++ += (l + r) * 0.5f;
                }

                sourceSamplePosition += pitchRatio;

                if (sourceSamplePosition > playingSound->length)
                {
                    stopNote(0.0f, false);
                    break;
                }
            }
        }
    }
    using SynthesiserVoice::renderNextBlock;

private:
    //==============================================================================
    double pitchRatio = 0;
    double downSampleRatio = 2; // only read every other sample
    int downSampleIncrement = 1;
    double sourceSamplePosition = 0;
    float lgain = 0, rgain = 0;

    ADSR adsr;

    JUCE_LEAK_DETECTOR(RomplerVoice)
};