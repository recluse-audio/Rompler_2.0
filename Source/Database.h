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
        
    }
    ~Database() {}

    void initFiles()
    {
        reset();
        auto artistFolders = juce::File(rootPath).findChildFiles(File::findDirectories, false);


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
                    filePaths.add(romples[k].getFullPathName());
                    category->addRomple(romples[k].getFileNameWithoutExtension());
                    fileNames.add(romples[k].getFileNameWithoutExtension());
                }
            }

            artists.add(artist);
        }
    }

    StringRef getFilePathFromIndex(int index) 
    { 

        return filePaths.getReference(index); 
    }

    StringRef getFileNameFromIndex(int index)
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

    void setRootPath(bool isWindows)
    {
		if (isWindows)
		{
			rootPath = { "C:/ProgramData/Recluse-Audio/Rompler/Romples/" };
		}
		else
		{
			rootPath = { " /Library/Application Support/Recluse-Audio/Rompler/Romples/" };
		}
    }
    
private:
    juce::OwnedArray<Artist> artists;
    juce::StringArray filePaths;
    juce::StringArray fileNames;
    juce::StringArray categoryNames;

    String rootPath; // set based on Mac or Windows OS

    friend class RompMenu;

};