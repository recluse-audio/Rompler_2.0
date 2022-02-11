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
    DrawableComponent(RomplerAudioProcessor& p) : audioProcessor(p)
    {
        selectDrawable(0);
    }

    ~DrawableComponent() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        auto alpha = audioProcessor.getRMSValue(0) + audioProcessor.getRMSValue(1);
        logo->setTransformToFit(getLocalBounds().toFloat(), RectanglePlacement::centred);
        logo->drawAt(g, 0, 0, 0.1f + alpha);
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
               xmlData = BinaryData::LOGO_SVG_svg;
               break;
        }
        std::unique_ptr<XmlElement> svg = XmlDocument::parse(BinaryData::LOGO_SVG_svg);
        jassert(svg != nullptr);

        if (svg != nullptr)
        {
           // auto path = svg->getChildByName("g")->getChildByName("path");
            //String newStyle = { "fill:#ffffff;fill-opacity:1;stroke-width:0.264583" };

          //  path->setAttribute("style", newStyle);


            logo = Drawable::createFromSVG(*svg);
            logo->setAlwaysOnTop(true);
        }
    }

private:
    std::unique_ptr<Drawable> logo;

    RomplerAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrawableComponent)
};
