/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RomplerAudioProcessor::RomplerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ), mAPVTS (*this, nullptr, "Parameters", createParameters())
#endif
{
    mFormatManager.registerBasicFormats();
    mAPVTS.state.addListener (this);
    
    for (int i = 0; i < mNumVoices; i++)
    {
        mSampler.addVoice (new RomplerVoice());
    }


}

RomplerAudioProcessor::~RomplerAudioProcessor()
{
    mAPVTS.state.removeListener (this);
}

//==============================================================================
const String RomplerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RomplerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RomplerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RomplerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RomplerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RomplerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RomplerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RomplerAudioProcessor::setCurrentProgram (int index)
{
}

const String RomplerAudioProcessor::getProgramName (int index)
{
    return {};
}

void RomplerAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void RomplerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mSampler.setCurrentPlaybackSampleRate (sampleRate);
    updateADSR();
}

void RomplerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RomplerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void RomplerAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    if (mShouldUpdate)
    {
        updateADSR();
    }
    
    MidiMessage m;
    MidiBuffer::Iterator it { midiMessages };
    int sample;
    
    while (it.getNextEvent (m, sample))
    {
        if (m.isNoteOn())
            mIsNotePlayed = true;
        else if (m.isNoteOff())
            mIsNotePlayed = false;
    }

    auto pitchRatio = std::pow(2.0, (m.getNoteNumber() - 60) / 12.0);
    auto sampleIncrement = buffer.getNumSamples() * pitchRatio;
    
    mSampleCount = mIsNotePlayed ? mSampleCount += sampleIncrement : 0;
    
    mSampler.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());
    
    auto channelRMSArray = Array<float>();
    smoothedRMSArray.clearQuick();
    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        float rms = buffer.getRMSLevel(channel, 0, buffer.getNumSamples());
        smoothedRMSArray.insert(channel, rms);
    }

}

//==============================================================================
bool RomplerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* RomplerAudioProcessor::createEditor()
{
    return new RomplerAudioProcessorEditor (*this);
}

//==============================================================================
void RomplerAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RomplerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}


void RomplerAudioProcessor::loadFile (const String& path)
{
    mSampler.clearSounds();
    
    auto file = File (path);
    // the reader can be a local variable here since it's not needed by the other classes after this
    std::unique_ptr<AudioFormatReader> reader{ mFormatManager.createReaderFor(file) };

    auto sampleDur = reader->lengthInSamples / reader->sampleRate;
    if (reader)
    {
        BigInteger range;
        range.setRange(0, 128, true);
        mSampler.addSound(new RomplerSound("Sample", *reader, range, 60, 0.1, 0.1, sampleDur));
        updateADSR();
    }
    
}

AudioBuffer<float>& RomplerAudioProcessor::getWaveForm()
{
    // get the last added synth sound as a SamplerSound*
    auto sound = dynamic_cast<RomplerSound*>(mSampler.getSound(mSampler.getNumSounds() - 1).get());
    if (sound)
    {
        return *sound->getAudioData();
    }
    // just in case it somehow happens that the sound doesn't exist or isn't a SamplerSound,
    // return a static instance of an empty AudioBuffer here...
    static AudioBuffer<float> dummybuffer;
    return dummybuffer;
}



float RomplerAudioProcessor::getRMSValue(int channel)
{

    return smoothedRMSArray[channel];
}

void RomplerAudioProcessor::flipStereoRomple()
{
    auto sound = dynamic_cast<RomplerSound*>(mSampler.getSound(mSampler.getNumSounds() - 1).get());
    if (sound)
    {
        auto rompleBuffer = sound->getAudioData();
        AudioBuffer<float> tempBuffer(*rompleBuffer);
        tempBuffer.makeCopyOf(*rompleBuffer);

        if (rompleBuffer->getNumChannels() > 1)
        {
            rompleBuffer->clear();
            rompleBuffer->copyFrom(0, 0, tempBuffer.getReadPointer(1), tempBuffer.getNumSamples());
            rompleBuffer->copyFrom(1, 0, tempBuffer.getReadPointer(0), tempBuffer.getNumSamples());
            updateADSR();

        }
    }
}

void RomplerAudioProcessor::reverseRomple()
{
    auto sound = dynamic_cast<RomplerSound*>(mSampler.getSound(mSampler.getNumSounds() - 1).get());
    if (sound)
    {
        auto rompleBuffer = sound->getAudioData();
        rompleBuffer->reverse(0, rompleBuffer->getNumSamples());
    }
}

void RomplerAudioProcessor::setRompleName(StringRef name)
{
    rompleName = name;
}

StringRef RomplerAudioProcessor::getRompleName()
{
    return rompleName;
}

void RomplerAudioProcessor::setSmoothedRMSValue(Array<float> channelArray)
{

}

AudioProcessorValueTreeState::ParameterLayout RomplerAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    params.push_back (std::make_unique<AudioParameterFloat>("ATTACK", "Attack", 0.0f, 2.0f, 0.01f));
    params.push_back (std::make_unique<AudioParameterFloat>("DECAY", "Decay", 0.0f, 2.0f, 2.0f));
    params.push_back (std::make_unique<AudioParameterFloat>("SUSTAIN", "Sustain", 0.0f, 1.0f, 1.0f));
    params.push_back (std::make_unique<AudioParameterFloat>("RELEASE", "Release", 0.0f, 2.0f, 0.1f));
    
    return { params.begin(), params.end() };
}

void RomplerAudioProcessor::updateADSR()
{
    mShouldUpdate = false;

    mADSRParams.attack = mAPVTS.getRawParameterValue("ATTACK")->load();
    mADSRParams.decay = mAPVTS.getRawParameterValue("DECAY")->load();
    mADSRParams.sustain = mAPVTS.getRawParameterValue("SUSTAIN")->load();
    mADSRParams.release = mAPVTS.getRawParameterValue("RELEASE")->load();

    for (int i = 0; i < mSampler.getNumSounds(); ++i)
    {
        if (auto sound = dynamic_cast<RomplerSound*>(mSampler.getSound(i).get()))
        {
            sound->setEnvelopeParameters(mADSRParams);
        }
    }
}

void RomplerAudioProcessor::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
{
    mShouldUpdate = true;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RomplerAudioProcessor();
}
