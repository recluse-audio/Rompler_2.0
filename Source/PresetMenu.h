/*
  ==============================================================================

    PresetMenu.h
    Created: 13 Jan 2022 10:28:57am
    Author:  ryand

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PulsarUIFeel.h"
#include "PluginProcessor.h"

//==============================================================================
/*
*  Made to handle saving and loading presets.  Hoping to integrate this and 'RompMenu'
*   Into a parent or rather base class Menu
*       
*/
class PresetMenu : public juce::Component, Button::Listener
{
public:
    PresetMenu(RomplerAudioProcessor& p) : audioProcessor(p)
    {
        if (audioProcessor.checkIfWindows())
        {
            rootPath = { "C:/ProgramData" };
        }
        else
        {
            rootPath = { "/Library/Application Support" };
        }
        setLookAndFeel(&pulsarFeel);
        prepareMenu();
        prepareCategoryMenu();

        presetNameInput = std::make_unique<Label>();
        presetNameInput->setText("Preset Name", NotificationType::dontSendNotification);
        presetNameInput->setColour(Label::backgroundColourId, Colours::darkorange.darker(0.9f));
        presetNameInput->setColour(Label::backgroundWhenEditingColourId, Colours::darkorange.darker(0.9f));
        presetNameInput->setLookAndFeel(&pulsarFeel);// (Font("Consolas", "Regular", 10.f));
        addChildComponent(presetNameInput.get());
        presetNameInput->setEditable(true);
        presetNameInput->onTextChange = [this] { createNewPreset(presetNameInput->getText()); };

        rompleNameInput = std::make_unique<Label>();
        rompleNameInput->setText("Romple Name", NotificationType::dontSendNotification);
        rompleNameInput->setColour(Label::backgroundColourId, Colours::darkorange.darker(0.9f));
        rompleNameInput->setColour(Label::backgroundWhenEditingColourId, Colours::darkorange.darker(0.9f));
        addChildComponent(rompleNameInput.get());
        rompleNameInput->setEditable(true);
        rompleNameInput->onTextChange = [this] { createNewRomple(rompleNameInput->getText()); };

        categoryNameInput = std::make_unique<Label>();
        categoryNameInput->setText("Romple Category", NotificationType::dontSendNotification);
        categoryNameInput->setColour(Label::backgroundColourId, Colours::darkorange.darker(0.9f));
        categoryNameInput->setColour(Label::backgroundWhenEditingColourId, Colours::aquamarine.darker(0.9f));
        categoryNameInput->setColour(Label::outlineColourId, Colours::darkorange.darker(0.9f));
        addChildComponent(categoryNameInput.get());
        categoryNameInput->setEditable(true);
        categoryNameInput->onTextChange = [this] { createNewCategory(categoryNameInput->getText()); };

        saveRompleButton = std::make_unique<TextButton>("Save Romple");
        saveRompleButton->setColour(TextButton::ColourIds::buttonColourId, Colours::darkorange.darker(0.9f));
        saveRompleButton->addListener(this);
        addChildComponent(saveRompleButton.get());

        savePresetButton = std::make_unique<TextButton>("Save Preset");
        savePresetButton->setColour(TextButton::ColourIds::buttonColourId, Colours::darkorange.darker(0.9f));
        savePresetButton->addListener(this);
        addChildComponent(savePresetButton.get());

        selectCategoryButton = std::make_unique<TextButton>("Select Category");
        selectCategoryButton->setColour(TextButton::ColourIds::buttonColourId, Colours::aquamarine.withAlpha(0.5f));
        selectCategoryButton->addListener(this);
        addChildComponent(selectCategoryButton.get());
            
    }

    ~PresetMenu() override
    {
        menu.clear();
        categoryMenu.clear();
    }

    void paint (juce::Graphics& g) override
    {
        g.setColour(juce::Colours::transparentBlack.withAlpha(0.f));
        //g.setColour(Colours::pink);
        g.fillAll();
    }

    void resized() override
    {
        sendLookAndFeelChange();
        rompleNameInput->setBoundsRelative(0.f, 0.f, 0.6f, 0.2f);
        categoryNameInput->setBoundsRelative(0.f, 0.2f, 0.6f, 0.2f);
        selectCategoryButton->setBoundsRelative(0.6f, 0.2f, 0.4f, 0.2f);
        presetNameInput->setBoundsRelative(0.f, 0.4f, 0.6f, 0.2f);
        savePresetButton->setBoundsRelative(0.f, 0.6f, 0.6f, 0.2f);
        saveRompleButton->setBoundsRelative(0.f, 0.4f, 0.3f, 0.2f);
        //menuButton->setBoundsRelative(0.f, 0.0f, 1.f, 1.f);

    }

    void buttonClicked(Button* b) override
    {
        if (b == savePresetButton.get())
        {
            presetToProcessor();
        }
        if (b == saveRompleButton.get())
        {
            rompleToProcessor();
        }
        if (b == selectCategoryButton.get())
        {
            showCategoryMenu();
        }
    }

    void showPresetMenu()
    {
		auto menuArea = Rectangle<int>(getScreenX(), getScreenY() - getParentHeight(), getParentWidth(), getParentHeight());

		if (audioProcessor.checkIfWindows())
		{
            int selection = menu.showMenu(PopupMenu::Options().withTargetScreenArea(menuArea));
            runPresetMenu(selection);
        }
        else
        {
			menu.showMenuAsync(PopupMenu::Options().withTargetScreenArea(menuArea),
                [this](int selection)
                {
                    runPresetMenu(selection);
                }
        }

       
        repaint();
    }

    // runs the selected menu item, separated to enable the different algorithms JUCE uses on different OS
    void runPresetMenu(int selection)
    {
		switch (selection)
		{
		case 0:
		{
			menu.dismissAllActiveMenus();
			setVisible(false);
		} break;
		case 1:
		{
			// remove user option to save existing romple
			if (audioProcessor.checkIfUserRomple())
			{
				rompleNameInput->setVisible(true);
				rompleNameInput->setText(audioProcessor.getRompleName(), NotificationType::dontSendNotification);
				categoryNameInput->setVisible(true);
				selectCategoryButton->setVisible(true);
			}

			presetNameInput->setVisible(true);
			savePresetButton->setVisible(true);

		} break;
		case 2:
		{
			// remove user option to save existing romple
			if (audioProcessor.checkIfUserRomple())
			{
				rompleNameInput->setVisible(true);
				rompleNameInput->setText(audioProcessor.getRompleName(), NotificationType::dontSendNotification);
				categoryNameInput->setVisible(true);
				selectCategoryButton->setVisible(true);
				saveRompleButton->setVisible(true);
			}
		} break;
		}

		if (selection > 2)
		{
			audioProcessor.loadPreset(presetPaths[selection]);
		}
    }
    // selection of folders to save a preset to
    void showCategoryMenu()
    {
        auto menuArea = Rectangle<int>(getScreenX() + getWidth(), getScreenY() + (getParentHeight() * 0.45f), getParentWidth(), getParentHeight());
        
        int selection = categoryMenu.showMenu(PopupMenu::Options().withTargetScreenArea(menuArea));

        if (audioProcessor.checkIfWindows())
        {

        }
        if (selection == 0)
        {
        }
        if (selection > 0)
        {
            categoryName = categoryNames[selection - 1];
            categoryNameInput->setText(categoryName, NotificationType::dontSendNotification);
        }

    }


    void prepareMenu()
    {
        menu.clear();
        menu.setLookAndFeel(&pulsarFeel);
        menu.addSectionHeader("SAVE");
        
        int itemIndex = 1;

        // Saving Presets and Saving Romples
        auto saveMenu = new PopupMenu();
        saveMenu->addItem(itemIndex, "Save Preset");
        itemIndex++;
        saveMenu->addItem(itemIndex, "Save Romple");
        itemIndex++;
        
        
        auto presetMenu = new PopupMenu();

        String presetPath = rootPath + String("/Recluse-Audio/Rompler/Presets/Stock Presets/");
        auto presetFiles = File(presetPath).findChildFiles(File::findFiles, false);

        for (int i = 0; i < presetFiles.size(); i++)
        {
            presetMenu->addItem(itemIndex, presetFiles[i].getFileNameWithoutExtension());
            presetPaths.set(itemIndex, presetFiles[i].getFullPathName());
            itemIndex++;
        }

        auto userPresetMenu = new PopupMenu();
        String userPresetPath = rootPath + String("/Recluse-Audio/Rompler/Presets/User Presets/");
        auto userPresetFiles = juce::File(userPresetPath).findChildFiles(File::findFiles, false);
        for (int j = 0; j < userPresetFiles.size(); j++)
        {
            userPresetMenu->addItem(itemIndex, userPresetFiles[j].getFileNameWithoutExtension());
            presetPaths.set(itemIndex, userPresetFiles[j].getFullPathName());
            itemIndex++;
        }

        menu.addSubMenu("SAVE", *saveMenu);
        menu.addSeparator();
        menu.addSectionHeader("LOAD");
        menu.addSubMenu("Presets", *presetMenu);
        menu.addSubMenu("User Presets", *userPresetMenu);

    }

    void prepareCategoryMenu()
    {
        categoryMenu.clear();
        categoryMenu.setLookAndFeel(&pulsarFeel);
        categoryMenu.addSectionHeader("Add To Romple Category");

        int itemIndex = 1;

        String categoryPath = rootPath + String("/Recluse-Audio/Rompler/Romples/User Romples/");
        auto categoryDirectories = File(categoryPath).findChildFiles(File::findDirectories, false);

        for (int i = 0; i < categoryDirectories.size(); i++)
        {
            categoryNames.set(i, categoryDirectories[i].getFileNameWithoutExtension());
            categoryMenu.addItem(itemIndex, categoryDirectories[i].getFileNameWithoutExtension());
            itemIndex++;
        }

    }

    void createNewRomple(StringRef rompName)
    {
        rompleName = rompName;
        rompleNameInput->setText("Romple Name:" + rompleName, NotificationType::dontSendNotification);
    }

    void createNewCategory(StringRef catName)
    {
        categoryName = catName;
        categoryNameInput->setText("Category Name: " + categoryName, NotificationType::dontSendNotification);
    }

    // called by the 'textInput' label as part of its text changed lambda
    void createNewPreset(StringRef preName)
    {
        sendLookAndFeelChange();
        presetName = preName;
        presetNameInput->setText("Preset Name: " + presetName, NotificationType::dontSendNotification);
    }

    // save romple and category to file structure in program data
    void rompleToProcessor()
    {
        audioProcessor.saveRomple(rompleName, categoryName);
        resetTextInput();
    }

    // save romple / category to file structure and preset to xml then files
    void presetToProcessor()
    {
        if(audioProcessor.checkIfUserRomple())
            audioProcessor.saveRomple(rompleName, categoryName);

        audioProcessor.savePreset(presetName);
        resetTextInput();
    }

    void resetTextInput()
    {
        categoryNameInput->setVisible(false);
        categoryNameInput->hideEditor(true);

        rompleNameInput->hideEditor(true);
        rompleNameInput->setVisible(false);

        presetNameInput->hideEditor(true);
        presetNameInput->setVisible(false);

        categoryMenu.dismissAllActiveMenus();

        prepareMenu();
        prepareCategoryMenu();
        setVisible(false);

    }

    bool rompleExists(StringRef rompName)
    {
        
    }
private:
    bool hasRompleName = false;
    String rompleName = { "" };
    StringArray categoryNames;
    String categoryName = { "" };
    String presetName = { "" };
    StringArray presetPaths = {"", "", ""}; // nasty way of offsetting because menu starts referencing these at index 3

    String rootPath; // set according to OS

    PopupMenu menu;
    PopupMenu categoryMenu;
    PulsarUIFeel pulsarFeel;

    std::unique_ptr<Label> presetNameInput;
    std::unique_ptr<Label> rompleNameInput;
    std::unique_ptr<Label> categoryNameInput;

    std::unique_ptr<TextButton> savePresetButton;
    std::unique_ptr<TextButton> saveRompleButton;
    std::unique_ptr<TextButton> selectCategoryButton;

    RomplerAudioProcessor& audioProcessor;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetMenu)
};
