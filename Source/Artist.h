/*
  ==============================================================================

    Artist.h
    Created: 24 Jul 2021 5:06:59pm
    Author:  ryand

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class Artist
{
public:
    Artist() {}
    Artist(StringRef name) : artistName(name) {}
    ~Artist() {}

    class Category
    {
    public:
        Category(StringRef name) : categoryName(name) {}
        ~Category() {}

        void addRomple(StringRef name)
        {
            rompleNames.add(name);
        }

        StringRef getName()
        {
            return categoryName;
        }

        StringRef getRomple(int index)
        {
            return rompleNames[index];
        }

        StringArray& getRompleArray()
        {
            return rompleNames;
        }

    private:
        String categoryName;
        StringArray rompleNames;
    };

    void setArtistName(StringRef name) { artistName = name; }

    void addCategory(StringRef name) 
    { 
        categories.add(new Category(name)); 
    }
    
    void addRomple(StringRef category, StringRef romple) 
    { 
        auto mCategory = getCategory(category);
        mCategory.addRomple(romple);
    }

    StringRef getArtistName() { return artistName; }

    /*
        meant for matching based on string
    */
    Category& getCategory(StringRef name)
    {
        for (int i = 0; i < categories.size(); i++)
        {
            if (categories[i]->getName() == name)
            {
                return *categories[i];
            }
        }
    }

    Category& getCategory(int index)
    {
        return *categories[index];
    }

    StringRef getCategoryName(int index)
    {
        return categories[index]->getName();
    }

    OwnedArray<Category>& getCategories()
    {
        return categories;
    }

    int getNumCategories()
    {
        return categories.size();
    }

private:
    juce::String artistName;

    OwnedArray<Category> categories;
};

