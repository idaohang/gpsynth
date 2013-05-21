/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "GPPluginProcessor.h"
#include "GPPluginEditor.h"

AudioProcessor* JUCE_CALLTYPE createPluginFilter();


//==============================================================================
/** A demo synth sound that's just a basic sine wave.. */
class GPSound : public SynthesiserSound
{
public:
    GPSound() {}

    bool appliesToNote (const int /*midiNoteNumber*/)           {
        return true;
    }
    bool appliesToChannel (const int /*midiChannel*/)           {
        return true;
    }
};

//==============================================================================
/** A simple demo synth voice that just plays a sine wave.. */
class GPVoice  : public SynthesiserVoice
{
public:
    GPVoice(GPNetwork* net, unsigned nv)
        : network(net),
		  buffer(nullptr),
		  numVariables(nv), variables(nullptr),
          blockSize(0), sampleTimes(nullptr), blockSizeInBytes(0), sampleRate(0),
		  frameNumber(0), cps(0.0), playing(false), level(0.0), tailOff(0.0)
    {
    }
	
	~GPVoice()
	{
		network->doneRendering();

        if (variables != nullptr)
			free(variables);
        if (buffer != nullptr)
			free(buffer);
		if (sampleTimes != nullptr)
			free(sampleTimes);
	}

	void setRenderParams(float sr, unsigned samplesPerBlock, float maxLen) {
		network->doneRendering();

		// set sample rate
		sampleRate = sr;

		// set block size
		blockSize = samplesPerBlock;
		blockSizeInBytes = sizeof(float) * blockSize;
		
		// set sample times
		if (sampleTimes != nullptr)
			free(sampleTimes);
		maxNumberOfFrames = (unsigned) maxLen * sr;
		sampleTimes = (float*) malloc(sizeof(float) * maxNumberOfFrames);
		for (unsigned i = 0; i < maxNumberOfFrames; i++) {
			sampleTimes[i] = i / (double) sr;
		}

		// prepare network
		network->prepareToRender(sr, blockSize, maxLen);

		// prepare other arrays
        if (variables != nullptr)
			free(variables);
        if (buffer != nullptr)
			free(buffer);
		buffer = (float*) malloc(sizeof(float) * blockSize);
		variables = (float*) malloc(sizeof(float) * numVariables);
	}

    void startNote (const int midiNoteNumber, const float velocity,
                    SynthesiserSound* /*sound*/, const int /*currentPitchWheelPosition*/)
    {
		// fill time info
		frameNumber = 0;

		// fill note info
        cps = (float) MidiMessage::getMidiNoteInHertz (midiNoteNumber);
		variables[0] = cps;

		// fill synth note state
		playing = true;
		level = velocity * 0.15;
		tailOff = 0.0;
    }

    void stopNote (const bool allowTailOff)
    {
        if (allowTailOff)
        {
            // start a tail-off by setting this flag. The render callback will pick up on
            // this and do a fade out, calling clearCurrentNote() when it's finished.

            if (tailOff == 0.0) // we only need to begin a tail-off if it's not already doing so - the
                // stopNote method could be called more than once.
                tailOff = 1.0;
        }
        else
        {
            // we're being told to stop playing immediately, so reset everything..

            clearCurrentNote();
        }
    }

    void pitchWheelMoved (const int /*newValue*/)
    {
        // can't be bothered implementing this for the demo!
    }

    void controllerMoved (const int /*controllerNumber*/, const int /*newValue*/)
    {
        // not interested in controllers in this case.
    }

    void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
    {
		if (playing) {
			// fill info buffers
			if (tailOff > 0) {
				playing = false;
				clearCurrentNote();
			}
			else {			
				// fill audio buffers
				network->evaluateBlockPerformance(frameNumber, numSamples, sampleTimes + frameNumber, numVariables, variables, buffer);

				for (int i = 0; i < outputBuffer.getNumChannels(); i++) {
					float* channelBuffer = outputBuffer.getSampleData(i, startSample);
					//memcpy(outputBuffer.getSampleData(i, startSample), buffer, blockSizeInBytes);
					for (int j = 0; j < numSamples; j++) {
						channelBuffer[j] = buffer[j] * level;
					}
				}
				frameNumber += numSamples;
			}
		}
    }

    bool canPlaySound (SynthesiserSound* sound)
    {
        return dynamic_cast <GPSound*> (sound) != 0;
    }

private:
	// Algorithm
    GPNetwork* network;

	// Render Buffer
	float* buffer;
	
	// Control Variables
    unsigned numVariables;
    float* variables;
	
	// Render Parameters
    int blockSize;
	unsigned maxNumberOfFrames;
    float* sampleTimes;
	size_t blockSizeInBytes;
	double sampleRate;

	// Current note info
	unsigned frameNumber;
    float cps;
	bool playing;
    float level;
	double tailOff;
};


//==============================================================================
GeneticProgrammingSynthesizerAudioProcessor::GeneticProgrammingSynthesizerAudioProcessor()
    //: delayBuffer (2, 12000)
{
    // Set up some default values..
	algorithm = 0;
	algorithmFitness = 0;
    gain = 1.0f;
	fitnesses = (double*) malloc(sizeof(double) * POPULATIONSIZE);
	for (unsigned i = 0; i < POPULATIONSIZE; i++) {
		fitnesses[i] = 0.0f;
	}

    lastUIWidth = 400;
    lastUIHeight = 478;

    lastPosInfo.resetToDefault();
    //delayPosition = 0;

	unsigned seed = 0;
	GPRandom* rng = new GPRandom(seed);
	// TODO: this is backwards, wtf
	GPNetwork* sinwave = new GPNetwork(rng, "(osc {d 0 0 1} {c 0.0 1.0 10.0})");
    sinwave->traceNetwork();

    // Initialise the synth...
    numSynthVoices = 4;
    synthVoices = (GPVoice**) malloc(sizeof(GPVoice*) * numSynthVoices);
    for (int i = numSynthVoices; --i >= 0;) {
        GPNetwork* sinCopy = sinwave->getCopy("clone");
        sinCopy->traceNetwork();
        GPVoice* newVoice = new GPVoice(sinCopy, 0);
        synth.addVoice (newVoice);   // These voices will play our custom sine-wave sounds..
        synthVoices[i] = newVoice;
    }

    synth.addSound (new GPSound());
}

GeneticProgrammingSynthesizerAudioProcessor::~GeneticProgrammingSynthesizerAudioProcessor()
{
	free(fitnesses);
}

//==============================================================================
int GeneticProgrammingSynthesizerAudioProcessor::getNumParameters()
{
    return totalNumParams;
}

float GeneticProgrammingSynthesizerAudioProcessor::getParameter (int index)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
	case algorithmParam:
		return (float) algorithm;
	case algorithmFitnessParam:
		return algorithmFitness;
    case gainParam:
        return gain;
    default:
        return 0.0f;
    }
}

void GeneticProgrammingSynthesizerAudioProcessor::setParameter (int index, float newValue)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
	case algorithmParam:
		algorithm = (unsigned) newValue;
	case algorithmFitnessParam:
		algorithmFitness = newValue;
		fitnesses[algorithm] = (double) newValue;
    case gainParam:
        gain = newValue;
    default:
        break;
    }
}

const String GeneticProgrammingSynthesizerAudioProcessor::getParameterName (int index)
{
    switch (index)
    {
	case algorithmParam:
		return "algorithm";
	case algorithmFitnessParam:
		return "algorithmFitness";
    case gainParam:
        return "gain";
    default:
        break;
    }

    return String::empty;
}

const String GeneticProgrammingSynthesizerAudioProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

//==============================================================================
void GeneticProgrammingSynthesizerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synth.setCurrentPlaybackSampleRate (sampleRate);

	float maxNoteLengthInSeconds = 10.0f;

    for (unsigned i = 0; i < numSynthVoices; i++) {
      synthVoices[i]->setRenderParams(sampleRate, samplesPerBlock, maxNoteLengthInSeconds);
    }

    keyboardState.reset();
    //delayBuffer.clear();
}

void GeneticProgrammingSynthesizerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    keyboardState.reset();
}

void GeneticProgrammingSynthesizerAudioProcessor::reset()
{
    // Use this method as the place to clear any delay lines, buffers, etc, as it
    // means there's been a break in the audio's continuity.
    //delayBuffer.clear();
}

void GeneticProgrammingSynthesizerAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int numSamples = buffer.getNumSamples();
    int channel, dp = 0;

    // Now pass any incoming midi messages to our keyboard state object, and let it
    // add messages to the buffer if the user is clicking on the on-screen keys
    keyboardState.processNextMidiBuffer (midiMessages, 0, numSamples, true);

    // and now get the synth to process these midi events and generate its output.
    synth.renderNextBlock (buffer, midiMessages, 0, numSamples);

	// Go through the incoming data, and apply our gain to it...
	/*
    for (channel = 0; channel < getNumInputChannels(); ++channel)
        buffer.applyGain (channel, 0, buffer.getNumSamples(), gain);
		*/

	/*
    // Apply our delay effect to the new output..
    for (channel = 0; channel < getNumInputChannels(); ++channel)
    {
        float* channelData = buffer.getSampleData (channel);
        float* delayData = delayBuffer.getSampleData (jmin (channel, delayBuffer.getNumChannels() - 1));
        dp = delayPosition;

        for (int i = 0; i < numSamples; ++i)
        {
            const float in = channelData[i];
            channelData[i] += delayData[dp];
            delayData[dp] = (delayData[dp] + in) * delay;
            if (++dp >= delayBuffer.getNumSamples())
                dp = 0;
        }
    }

    delayPosition = dp;
	*/

    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // ask the host for the current time so we can display it...
    AudioPlayHead::CurrentPositionInfo newTime;

    if (getPlayHead() != nullptr && getPlayHead()->getCurrentPosition (newTime))
    {
        // Successfully got the current time from the host..
        lastPosInfo = newTime;
    }
    else
    {
        // If the host fails to fill-in the current time, we'll just clear it to a default..
        lastPosInfo.resetToDefault();
    }
}

//==============================================================================
AudioProcessorEditor* GeneticProgrammingSynthesizerAudioProcessor::createEditor()
{
    return new GeneticProgrammingSynthesizerAudioProcessorEditor (this);
}

//==============================================================================
void GeneticProgrammingSynthesizerAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // Here's an example of how you can use XML to make it easy and more robust:

    // Create an outer XML element..
    XmlElement xml ("MYPLUGINSETTINGS");

    // add some attributes to it..
    xml.setAttribute ("uiWidth", lastUIWidth);
    xml.setAttribute ("uiHeight", lastUIHeight);
    xml.setAttribute ("gain", gain);

    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);
}

void GeneticProgrammingSynthesizerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
    {
        // make sure that it's actually our type of XML object..
        if (xmlState->hasTagName ("MYPLUGINSETTINGS"))
        {
            // ok, now pull out our parameters..
            lastUIWidth  = xmlState->getIntAttribute ("uiWidth", lastUIWidth);
            lastUIHeight = xmlState->getIntAttribute ("uiHeight", lastUIHeight);

            gain  = (float) xmlState->getDoubleAttribute ("gain", gain);
        }
    }
}

const String GeneticProgrammingSynthesizerAudioProcessor::getInputChannelName (const int channelIndex) const
{
    return String (channelIndex + 1);
}

const String GeneticProgrammingSynthesizerAudioProcessor::getOutputChannelName (const int channelIndex) const
{
    return String (channelIndex + 1);
}

bool GeneticProgrammingSynthesizerAudioProcessor::isInputChannelStereoPair (int /*index*/) const
{
    return true;
}

bool GeneticProgrammingSynthesizerAudioProcessor::isOutputChannelStereoPair (int /*index*/) const
{
    return true;
}

bool GeneticProgrammingSynthesizerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool GeneticProgrammingSynthesizerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool GeneticProgrammingSynthesizerAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double GeneticProgrammingSynthesizerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GeneticProgrammingSynthesizerAudioProcessor();
}
