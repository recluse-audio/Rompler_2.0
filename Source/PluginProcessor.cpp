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
                       ), mAPVTS (*this, nullptr, "Parameters", createParameters()), database()
#endif
{
    mFormatManager.registerBasicFormats();
    mAPVTS.state.addListener (this);
    
#if JUCE_WINDOWS
    {
        isWindows = true;
        rootPath = { "C:/ProgramData/" };
    }
#else
	{
		isWindows = false;
		rootPath = { "/Library/Application Support/" };
    }
#endif

    initUserPath();

    database.setRootPath(isWindows);
    database.initFiles();

    for (int i = 0; i < mNumVoices; i++)
    {
        mSampler.addVoice (new RomplerVoice());
    }

    loadFileSelection(0); // this talks to the apvts also (maybe move the apvts stuff here?)

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
    update();
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
        update();
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
    XmlElement* xml = new XmlElement(mainTag);

    std::unique_ptr<XmlElement> apvtsXml = mAPVTS.copyState().createXml();
    apvtsXml->setTagName(treeTag);

    std::unique_ptr<XmlElement> rompleXml = std::make_unique<XmlElement>(pathTag);
    rompleXml->setAttribute(pathAttribute, romplePath);
    rompleXml->setAttribute(flipAttribute, isFlipped);
    rompleXml->setAttribute(reverseAttribute, isReversed);

    // ..and add our new element to the parent node
    xml->addChildElement(apvtsXml.get());
    xml->addChildElement(rompleXml.get());

    copyXmlToBinary(*xml, destData);
}

void RomplerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto xml = getXmlFromBinary(data, sizeInBytes);

    auto apvtsXml = xml->getChildByName(treeTag);
    juce::ValueTree copyState = juce::ValueTree::fromXml(*apvtsXml);
    mAPVTS.replaceState(copyState);

    auto rompleXml = xml->getChildByName(pathTag);
    auto xmlPath = rompleXml->getStringAttribute(pathAttribute); // path to saved romple that needs to changed based on OS
    auto shortPath = xmlPath.fromFirstOccurrenceOf({ "Recluse" }, true, false);
    String loadPath = rootPath + shortPath;
    loadFile(loadPath);

    
    if (rompleXml->getBoolAttribute(flipAttribute))
        flipStereoRomple();
    isFlipped = rompleXml->getBoolAttribute(flipAttribute); // flipStereoRomple() also flips this state, so we hard code here to overrule the flip in the function

    if (rompleXml->getBoolAttribute(reverseAttribute))
        reverseRomple();
    isReversed = rompleXml->getBoolAttribute(reverseAttribute); // same as above

}

void RomplerAudioProcessor::saveRomple(StringRef rompleName, StringRef categoryName)
{
    String category = categoryName;
    category.append("/", 2);

    auto userCategory = File(userPath + category);
    auto userRompleDirectory = File(userPath);

    if (!userCategory.isAChildOf(userRompleDirectory)) // search user directory for existing category and then create a new directory if necessary
    {
        auto newCategory = File(userPath + category);
        newCategory.create();
        userCategory.copyDirectoryTo(newCategory);
    }

    auto file = juce::File(romplePath); // the current romple

    if (rompleName == String("Romple Name") || rompleName == String("")) // if user didn't enter a name just use the original name of the romple
    {
        String newPath = userPath + category + file.getFileName();
        romplePath = newPath; // store this because it gets saved in our xml
        auto newFile = File(newPath);
        newFile.create();
        file.copyFileTo(newFile);
    }
    else
    {
        String newPath = userPath + category + rompleName + file.getFileExtension();
        romplePath = newPath; // store this because it gets saved in our xml
        auto newFile = File(newPath);
        newFile.create();
        file.copyFileTo(newFile);
    }
   
    database.initFiles();
}

void RomplerAudioProcessor::savePreset(StringRef presetName)
{
    auto presetPath = userPresetPath + presetName;
    MemoryBlock block;
    auto file = juce::File(presetPath);
    getStateInformation(block); // copies xml state to this 'block' a binary blob
    file.replaceWithData(block.getData(), block.getSize());
}

void RomplerAudioProcessor::loadPreset(StringRef presetPath)
{
    MemoryBlock block;
    auto file = juce::File(presetPath);
    file.loadFileAsData(block); // fills block with files data.  See setStateInformation() in Audio Processor Base class
    setStateInformation(block.getData(), (int)block.getSize());
    mShouldUpdate = true;
}

void RomplerAudioProcessor::loadUserFile(File mFile)
{
    mSampler.clearSounds();

    rompleName = mFile.getFileNameWithoutExtension();
    romplePath = mFile.getFullPathName();

    // the reader can be a local variable here since it's not needed by the other classes after this
    std::unique_ptr<AudioFormatReader> reader{ mFormatManager.createReaderFor(mFile) };

    auto sampleDur = reader->lengthInSamples / reader->sampleRate;

    if (reader)
    {
        BigInteger range;
        range.setRange(0, 128, true);
        mSampler.addSound(new RomplerSound("Sample", *reader, range, 60, 0.1, 0.1, sampleDur));
        update();

        isUserRomple = true;
    }
}

void RomplerAudioProcessor::loadFileSelection(int selection)
{
    auto path = database.getFilePathFromIndex(selection); // item id's start at 1, but passing to a vector storign file paths at [0]
    romplePath = path;
    loadFile(path);
}


void RomplerAudioProcessor::loadFile (const String& path)
{
    mSampler.clearSounds();
    
    auto file = File (path);

    rompleName = file.getFileNameWithoutExtension();

    std::unique_ptr<AudioFormatReader> reader{ mFormatManager.createReaderFor(file) };

    auto sampleDur = reader->lengthInSamples / reader->sampleRate;

    if (reader)
    {
        BigInteger range;
        range.setRange(0, 128, true);
        mSampler.addSound(new RomplerSound("Sample", *reader, range, 60, 0.1, 0.1, sampleDur));
        update();

        isUserRomple = false;
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
    isFlipped = !isFlipped; // clever flip, just for the xml

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
            update();

        }
    }
}

void RomplerAudioProcessor::reverseRomple()
{
    isReversed = !isReversed; // am I a fucking genius?  this flips the boolean right?

    auto sound = dynamic_cast<RomplerSound*>(mSampler.getSound(mSampler.getNumSounds() - 1).get());
    if (sound)
    {
        auto rompleBuffer = sound->getAudioData();
        rompleBuffer->reverse(0, rompleBuffer->getNumSamples());
    }
}

StringRef RomplerAudioProcessor::getRompleName()
{
    return rompleName;
}

bool RomplerAudioProcessor::checkIfUserRomple()
{
    return isUserRomple;
}

Database& RomplerAudioProcessor::getDatabase()
{
    return database;
}

bool RomplerAudioProcessor::checkIfWindows()
{
    return isWindows;
}

// Sets user path according to OS (windows/mac)
void RomplerAudioProcessor::initUserPath()
{
    if (isWindows)
    {
		userPath = { "C:/ProgramData/Recluse-Audio/Rompler/Romples/User Romples/" };
		userPresetPath = { "C:/ProgramData/Recluse-Audio/Rompler/Presets/User Presets/" };
    }
    else
    {
		userPath = { "/Library/Application Support/Recluse-Audio/Rompler/Romples/User Romples/" };
		userPresetPath = { "/Library/Application Support/Recluse-Audio/Rompler/Presets/User Presets/" };
    }

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

    params.push_back(std::make_unique<AudioParameterFloat>("File Selection", "File Selection", 0, 100, 5));
    
    return { params.begin(), params.end() };
}

void RomplerAudioProcessor::update()
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
