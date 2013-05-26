/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "GPPluginEditor.h"

//==============================================================================
GeneticProgrammingSynthesizerAudioProcessorEditor::GeneticProgrammingSynthesizerAudioProcessorEditor (GeneticProgrammingSynthesizerAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
	  mainEditor(),
      midiKeyboard (ownerFilter->keyboardState, MidiKeyboardComponent::horizontalKeyboard),
	  sliders(), buttons()
{
	// add main component
	addAndMakeVisible(&mainEditor);
	mainEditor.setSliderListener(this);
	mainEditor.setButtonListener(this);
	mainEditor.getSliders(sliders);
	mainEditor.getButtons(buttons);

	/*
    // add some sliders..
    addAndMakeVisible (&gainSlider);
    gainSlider.setSliderStyle (Slider::Rotary);
    gainSlider.addListener (this);
    gainSlider.setRange (0.0, 1.0, 0.01);

    addAndMakeVisible (&delaySlider);
    delaySlider.setSliderStyle (Slider::Rotary);
    delaySlider.addListener (this);
    delaySlider.setRange (0.0, 1.0, 0.01);

    // add some labels for the sliders..
     gainLabel.attachToComponent (&gainSlider, false);
    gainLabel.setFont (Font (11.0f));

    delayLabel.attachToComponent (&delaySlider, false);
    delayLabel.setFont (Font (11.0f));
		*/

    // add the midi keyboard component..
    addAndMakeVisible (&midiKeyboard);

	/*
    // add a label that will display the current timecode and status..
    addAndMakeVisible (&infoLabel);
    infoLabel.setColour (Label::textColourId, Colours::blue);
	*/

    // add the triangular resizer component for the bottom-right of the UI
    addAndMakeVisible (resizer = new ResizableCornerComponent (this, &resizeLimits));
    resizeLimits.setSizeLimits (400, 478, 400, 478);

    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize (ownerFilter->lastUIWidth,
             ownerFilter->lastUIHeight);

    startTimer (50);
}

GeneticProgrammingSynthesizerAudioProcessorEditor::~GeneticProgrammingSynthesizerAudioProcessorEditor()
{
}

//==============================================================================
void GeneticProgrammingSynthesizerAudioProcessorEditor::paint (Graphics& g)
{
    g.setGradientFill (ColourGradient (Colours::white, 0, 0, Colours::grey, 0, (float) getHeight(), false));
    g.fillAll();
}

void GeneticProgrammingSynthesizerAudioProcessorEditor::resized()
{
	/*
    infoLabel.setBounds (10, 4, 400, 25);
    gainSlider.setBounds (20, 60, 150, 40);
    delaySlider.setBounds (200, 60, 150, 40);
	*/

    const int keyboardHeight = 70;
	mainEditor.setBounds(0, 0, 400, 400);
    midiKeyboard.setBounds (4, getHeight() - keyboardHeight - 4, getWidth() - 8, keyboardHeight);

    //resizer->setBounds (getWidth() - 16, getHeight() - 16, 16, 16);

    getProcessor()->lastUIWidth = getWidth();
    getProcessor()->lastUIHeight = getHeight();
}

//==============================================================================
// This timer periodically checks whether any of the filter's parameters have changed...
void GeneticProgrammingSynthesizerAudioProcessorEditor::timerCallback()
{
    GeneticProgrammingSynthesizerAudioProcessor* ourProcessor = getProcessor();

    //AudioPlayHead::CurrentPositionInfo newPos (ourProcessor->lastPosInfo);

	/*
    if (lastDisplayedPosition != newPos)
        displayPositionInfo (newPos);
	*/

	/*
    gainSlider.setValue (ourProcessor->gain, dontSendNotification);
    delaySlider.setValue (ourProcessor->delay, dontSendNotification);
	*/

	sliders["algorithm"]->setValue(ourProcessor->algorithm, dontSendNotification);
	sliders["fitness"]->setValue(ourProcessor->algorithmFitness, dontSendNotification);
	sliders["gain"]->setValue(ourProcessor->gain, dontSendNotification);
}

// This is our Slider::Listener callback, when the user drags a slider.
void GeneticProgrammingSynthesizerAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
	// verified sliders are working here
    if (slider == sliders["algorithm"])
    {
		//appendToTextFile("./debug.txt", slider->getName() + String(" algorithm \n"));
        // It's vital to use setParameterNotifyingHost to change any parameters that are automatable
        // by the host, rather than just modifying them directly, otherwise the host won't know
        // that they've changed.
        getProcessor()->setParameterNotifyingHost (GeneticProgrammingSynthesizerAudioProcessor::algorithmParam,
                                                   (float) slider->getValue());
    }
    else if (slider == sliders["fitness"])
    {
		//appendToTextFile("./debug.txt", slider->getName() + String(" fitness \n"));
        getProcessor()->setParameterNotifyingHost (GeneticProgrammingSynthesizerAudioProcessor::algorithmFitnessParam,
                                                   (float) slider->getValue());
    }
	else if (slider == sliders["gain"])
	{
		//appendToTextFile("./debug.txt", slider->getName() + String(" gain \n"));
        getProcessor()->setParameterNotifyingHost (GeneticProgrammingSynthesizerAudioProcessor::gainParam,
                                                   (float) slider->getValue());
	}
}

void GeneticProgrammingSynthesizerAudioProcessorEditor::buttonClicked (Button* button)
{
	if (button == buttons["save"]) {
		getProcessor()->saveCurrentNetwork();
	}
	else if (button == buttons["load"]) {
		getProcessor()->loadReplacingCurrentNetwork();
	}
	else if (button == buttons["prevgen"]) {
		getProcessor()->prevGeneration();
	}
	else if (button == buttons["nextgen"]) {
		getProcessor()->nextGeneration();
	}
}

//==============================================================================
// quick-and-dirty function to format a timecode string
static const String timeToTimecodeString (const double seconds)
{
    const double absSecs = fabs (seconds);

    const int hours =  (int) (absSecs / (60.0 * 60.0));
    const int mins  = ((int) (absSecs / 60.0)) % 60;
    const int secs  = ((int) absSecs) % 60;

    String s (seconds < 0 ? "-" : "");

    s << String (hours).paddedLeft ('0', 2) << ":"
      << String (mins) .paddedLeft ('0', 2) << ":"
      << String (secs) .paddedLeft ('0', 2) << ":"
      << String (roundToInt (absSecs * 1000) % 1000).paddedLeft ('0', 3);

    return s;
}

// quick-and-dirty function to format a bars/beats string
static const String ppqToBarsBeatsString (double ppq, double /*lastBarPPQ*/, int numerator, int denominator)
{
    if (numerator == 0 || denominator == 0)
        return "1|1|0";

    const int ppqPerBar = (numerator * 4 / denominator);
    const double beats  = (fmod (ppq, ppqPerBar) / ppqPerBar) * numerator;

    const int bar    = ((int) ppq) / ppqPerBar + 1;
    const int beat   = ((int) beats) + 1;
    const int ticks  = ((int) (fmod (beats, 1.0) * 960.0 + 0.5));

    String s;
    s << bar << '|' << beat << '|' << ticks;
    return s;
}

// Updates the text in our position label.
/*
void GeneticProgrammingSynthesizerAudioProcessorEditor::displayPositionInfo (const AudioPlayHead::CurrentPositionInfo& pos)
{
    lastDisplayedPosition = pos;
    String displayText;
    displayText.preallocateBytes (128);

    displayText << String (pos.bpm, 2) << " bpm, "
                << pos.timeSigNumerator << '/' << pos.timeSigDenominator
                << "  -  " << timeToTimecodeString (pos.timeInSeconds)
                << "  -  " << ppqToBarsBeatsString (pos.ppqPosition, pos.ppqPositionOfLastBarStart,
                                                    pos.timeSigNumerator, pos.timeSigDenominator);

    if (pos.isRecording)
        displayText << "  (recording)";
    else if (pos.isPlaying)
        displayText << "  (playing)";

    infoLabel.setText (displayText, dontSendNotification);
}
*/
