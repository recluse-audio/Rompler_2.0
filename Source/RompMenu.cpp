/*
  ==============================================================================

    RompMenu.cpp
    Created: 23 Jul 2021 5:40:04pm
    Author:  ryand

  ==============================================================================
*/

#include <JuceHeader.h>
#include "RompMenu.h"

//==============================================================================
RompMenu::RompMenu(RomplerAudioProcessor& p) : audioProcessor(p), database()
{
    setLookAndFeel(&pulsarFeel);

    menuButton = std::make_unique<TextButton>("Romples");
    menuButton->addListener(this);
    addAndMakeVisible(menuButton.get());

    rompleLabel = std::make_unique<Label>("");
    addAndMakeVisible(rompleLabel.get());
    prepareMenu();
}

RompMenu::~RompMenu()
{
    menu.clear();
}

void RompMenu::paint (juce::Graphics& g)
{
    g.setColour(juce::Colours::transparentBlack.withAlpha(0.f));
    g.fillAll();

    //rompleLabel->setText(fileName, NotificationType::sendNotificationAsync);
    
}

void RompMenu::resized()
{
    menuButton->setBoundsRelative(0.8f, 0.05f, 0.175f, 0.7f);
    //rompleLabel->setBoundsRelative(0.05f, 0.05f, 0.3f, 0.7f);

}


void RompMenu::mouseDown(const juce::MouseEvent& e)
{
    //int selection = menu.showAt(this) - 1; // item id's start at 1, but passing to a vector storign file paths at [0]
    //auto filePath = database.getFilePathFromIndex(selection);
    //audioProcessor.loadFile(filePath);
}

void RompMenu::buttonClicked(Button* b)
{
    if (b == menuButton.get())
    {
        auto menuArea = Rectangle<int>(getScreenX(), getScreenY() - getParentHeight(), getParentWidth(), getParentHeight());
        int selection = menu.showMenu(PopupMenu::Options().withTargetScreenArea(menuArea));

        if (selection == 0)
        {
            menu.dismissAllActiveMenus();
        }
        if (selection > 0)
        {
            auto path = database.getFilePathFromIndex(selection - 1); // item id's start at 1, but passing to a vector storign file paths at [0]
            audioProcessor.loadFile(path);

            fileName = database.getFileNameFromIndex(selection - 1);
            audioProcessor.setRompleName(fileName);
        }
    }
    repaint();
}

void RompMenu::prepareMenu()
{
    menu.setLookAndFeel(&pulsarFeel);

    menu.addSectionHeader("Artists");
    menu.addSeparator();

    int itemIndex = 1; // used to properly index artist romples without resetting in the artist loop

    for (int artistIndex = 0; artistIndex < database.artists.size(); artistIndex++) // Artists - just a section header
    {
        auto artist = database.artists[artistIndex];
        menu.addSectionHeader(artist->getArtistName());

        for (int categoryIndex = 0; categoryIndex < artist->getNumCategories(); categoryIndex++)
        {

            auto categoryMenu = new PopupMenu();
            auto category = artist->getCategory(categoryIndex);
            auto romples = category.getRompleArray();

            for (int rompleIndex = 0; rompleIndex < romples.size(); rompleIndex++) 
            {
                categoryMenu->addItem(itemIndex, romples[rompleIndex]); // item index keeps track of the different categories
                itemIndex++;
            }

            menu.addSubMenu(category.getName(), *categoryMenu);
        }

    }

    
}

