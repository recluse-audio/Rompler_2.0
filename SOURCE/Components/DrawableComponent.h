/*
  ==============================================================================

    DrawableComponent.h
    Created: 9 Nov 2021 6:00:51pm
    Author:  ryand

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class DrawableComponent  : public juce::Component
{
public:
    DrawableComponent()
    {
        selectDrawable(0);
    }

    ~DrawableComponent() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        logo->setTransformToFit(getLocalBounds().toFloat(), RectanglePlacement::stretchToFit);
        logo->drawAt(g, 0, 0, 1.f);
    }

    void resized() override
    {

    }

    void selectDrawable(int index)
    {
        StringRef xmlData;

        switch (index)
        {
           case 0: 
               xmlData = BinaryData::LOGO_SVG_svg;
               break;
           case 1:
               xmlData = BinaryData::Spyder_Sound_svg;
               break;
        }
        std::unique_ptr<XmlElement> svg = XmlDocument::parse(BinaryData::LOGO_SVG_svg);
        jassert(svg != nullptr);

        if (svg != nullptr)
        {
            logo = Drawable::createFromSVG(*svg);
            logo->setAlwaysOnTop(true);
        }
    }

private:
    std::unique_ptr<Drawable> logo;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrawableComponent)
};
