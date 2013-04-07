/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef __PLUGINEDITOR_H_4ACCBAA__
#define __PLUGINEDITOR_H_4ACCBAA__

#include "../JuceLibraryCode/JuceHeader.h"
#include "GPPluginProcessor.h"


//==============================================================================
/** This is the editor component that our filter will display.
*/
class GeneticProgrammingSynthesizerAudioProcessorEditor  : public AudioProcessorEditor,
                                            public SliderListener,
                                            public Timer
{
public:
    GeneticProgrammingSynthesizerAudioProcessorEditor (GeneticProgrammingSynthesizerAudioProcessor* ownerFilter);
    ~GeneticProgrammingSynthesizerAudioProcessorEditor();

    //==============================================================================
    void timerCallback();
    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider*);

private:
    MidiKeyboardComponent midiKeyboard;
    Label infoLabel, gainLabel, delayLabel;
    Slider gainSlider;
    Slider delaySlider;
    ScopedPointer<ResizableCornerComponent> resizer;
    ComponentBoundsConstrainer resizeLimits;

    AudioPlayHead::CurrentPositionInfo lastDisplayedPosition;

    GeneticProgrammingSynthesizerAudioProcessor* getProcessor() const
    {
        return static_cast <GeneticProgrammingSynthesizerAudioProcessor*> (getAudioProcessor());
    }

    void displayPositionInfo (const AudioPlayHead::CurrentPositionInfo& pos);
};


#endif  // __PLUGINEDITOR_H_4ACCBAA__
