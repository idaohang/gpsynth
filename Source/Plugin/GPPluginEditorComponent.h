/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-13 by Raw Material Software Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_A5A671BF6408B931__
#define __JUCE_HEADER_A5A671BF6408B931__

//[Headers]     -- You can add your own extra header files here --
#include "GPPluginProcessor.h"
#include "../../JuceLibraryCode/JuceHeader.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class GeneticProgrammingSynthesizerComponent  : public Component,
                                                public SliderListener,
                                                public ButtonListener
{
public:
    //==============================================================================
    GeneticProgrammingSynthesizerComponent ();
    ~GeneticProgrammingSynthesizerComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void setSliderListener(SliderListener* sl);
	void setButtonListener(ButtonListener* bl);
	void getSliders(std::map<String, Slider*>& sliders);
	void getButtons(std::map<String, Button*>& buttons);
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);
    void buttonClicked (Button* buttonThatWasClicked);

    // Binary resources:
    static const char* sine_png;
    static const int sine_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	SliderListener* sliderListener;
	ButtonListener* buttonListener;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Slider> fitness;
    ScopedPointer<Slider> algonum;
    ScopedPointer<Label> title;
    ScopedPointer<Label> algolabel;
    ScopedPointer<Label> fitlabel;
    ScopedPointer<Label> gasetlabel;
    ScopedPointer<Label> playsetlabel;
    ScopedPointer<Slider> gain;
    ScopedPointer<Label> gainlabel;
    ScopedPointer<TextButton> save;
    ScopedPointer<ImageButton> logo;
    ScopedPointer<TextButton> load;
    ScopedPointer<Label> RHSlabel;
    ScopedPointer<TextButton> nextgen;
    ScopedPointer<TextButton> prevgen;
    ScopedPointer<TextButton> edit;
    ScopedPointer<TextButton> random;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GeneticProgrammingSynthesizerComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_A5A671BF6408B931__
