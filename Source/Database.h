/*
  ==============================================================================

    Database.h
    Created: 27 Jul 2021 3:05:01pm
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include "Artist.h"
#include <JuceHeader.h>
using namespace juce;
// This class is for intializing each artist and their romples.  This holds the array of file paths
// I wish I could get this to store file paths by artist, but returnign the selected index id from 
// the popup menu is simpler than searching for a key/value via the string of the menu seleciton
// See:  Rompler.h
class Database
{
public:
    Database() 
    { 
        initFiles(); 
    }
    ~Database() {}

    void initFiles()
    {
        reset();
        auto artistFolders = juce::File("/Library/Application Support/Recluse-Audio/Rompler/Romples/").findChildFiles(File::findDirectories, false);
        DBG(artistFolders.size());
 

        for (int i = 0; i < artistFolders.size(); i++)
        {
            auto artist = new Artist(artistFolders[i].getFileName());
            auto categories = artistFolders[i].findChildFiles(File::findDirectories, false);

            for (int j = 0; j < categories.size(); j++)
            {
                auto romples = categories[j].findChildFiles(File::findFiles, true);
                artist->addCategory(categories[j].getFileNameWithoutExtension());

                auto category = artist->getCategories()[j];

                for (int k = 0; k < romples.size(); k++)
                {
                    auto ext = romples[k].getFileExtension();
                    if( ext == ".wav" || ext == ".aif" || ext == ".mp3")
                    {
                        filePaths.add(romples[k].getFullPathName());
                        category->addRomple(romples[k].getFileNameWithoutExtension());
                        fileNames.add(romples[k].getFileNameWithoutExtension());
                    }
                }
            }

            artists.add(artist);
        }
    }

    juce::String& getFilePathFromIndex(int index) 
    { 

        return filePaths.getReference(index); 
    }

    juce::String& getFileNameFromIndex(int index)
    {
        return fileNames.getReference(index);
    }

    void reset()
    {
        artists.clear(true);
        filePaths.clear();
        fileNames.clear();
        categoryNames.clear();
    }
    
private:
    juce::OwnedArray<Artist> artists;
    juce::StringArray filePaths;
    juce::StringArray fileNames;
    juce::StringArray categoryNames;


    friend class RompMenu;
};
