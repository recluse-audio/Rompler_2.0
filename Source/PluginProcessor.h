/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RomplerSound.h"
#include "Database.h"

//==============================================================================
/**
*/
class RomplerAudioProcessor  : public AudioProcessor,
                                    public ValueTree::Listener
{
public:
    //==============================================================================
    RomplerAudioProcessor();
    ~RomplerAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void saveRomple(StringRef rompleName, StringRef categoryName); // saves current romple to user romples directory
    void savePreset(StringRef presetPath);
    void loadPreset(StringRef presetPath);
    void loadUserFile(File file);
    void loadFileSelection(int selection); // loads from apvts
    void loadFile (const String& path);
    
    int getNumSamplerSounds() { return mSampler.getNumSounds(); }
    AudioBuffer<float>& getWaveForm();
    
    void update();
    ADSR::Parameters& getADSRParams() { return mADSRParams; }
    
    AudioProcessorValueTreeState& getValueTree() { return mAPVTS; }
    std::atomic<bool>& isNotePlayed() { return mIsNotePlayed; }
    std::atomic<int>& getSampleCount() { return mSampleCount; }

    /* Used by WaveThumbnail for reactive effect*/
    float getRMSValue(int channel);

    void flipStereoRomple();
    void reverseRomple();

    StringRef getRompleName();
    bool checkIfUserRomple();

    Database& getDatabase();

    bool checkIfWindows();
    void initUserPath();
private:
    Database database;

    /* Setting smoothed value which will be retrieved from when getRMSValue() is called*/
    void setSmoothedRMSValue(Array<float> channelArray);
    Array <float> smoothedRMSArray;

    //OwnedArray <SmoothedValue<float>> smoothedRMSArray;

    Synthesiser mSampler;
    
    const int mNumVoices { 3 };
    
    ADSR::Parameters mADSRParams;
    
    AudioFormatManager mFormatManager;
    
    AudioProcessorValueTreeState mAPVTS;
    AudioProcessorValueTreeState::ParameterLayout createParameters();
    void valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property) override;
    
    std::atomic<bool> mShouldUpdate { false };
    std::atomic<bool> mIsNotePlayed { false };
    std::atomic<bool> isReversed    { false };
    std::atomic<bool> isFlipped     { false };
    std::atomic<int> mSampleCount { 0 };

    bool isUserRomple = false;

    String rompleName{ "" };
    String romplePath{ "" };
    String userPath;
    String userPresetPath;
    String rootPath; 

    String mainTag{ "romplerxml" };
    String treeTag{ "treexml" };
    String pathTag{ "pathxml" };
    String pathAttribute{ "pathattribute" };
    String flipAttribute{ "isflipped" };
    String reverseAttribute{ "isreversed" };

    bool isWindows = false;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RomplerAudioProcessor)
};
