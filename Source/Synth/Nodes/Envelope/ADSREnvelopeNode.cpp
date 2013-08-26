#include "ADSREnvelopeNode.h"

/*
    ========================
    CONSTRUCTION/DESTRUCTION
    ========================
*/

ADSREnvelopeNode::ADSREnvelopeNode(GPMutatableParam* del, GPMutatableParam* atk, GPMutatableParam* atkh, GPMutatableParam* dec, GPMutatableParam* sus, GPMutatableParam* sush, GPMutatableParam* rel, GPNode* signal)
{
    releaseFinished = false;
    framesInEnvelope = 0;
    envelope = NULL;
    sampleRate = 0;

    mutatableParams.push_back(del);
    mutatableParams.push_back(atk);
    mutatableParams.push_back(atkh);
    mutatableParams.push_back(dec);
    mutatableParams.push_back(sus);
    mutatableParams.push_back(sush);
    mutatableParams.push_back(rel);

    arity = 1;
    descendants.push_back(signal);

    symbol = "adsr*";
}

ADSREnvelopeNode::~ADSREnvelopeNode() {
    if (preparedToRender) {
        free(envelope);
    }
}

/*
    =========
    OVERRIDES
    =========
*/

ADSREnvelopeNode* ADSREnvelopeNode::getCopy() {
    return new ADSREnvelopeNode(mutatableParams[0]->getCopy(), mutatableParams[1]->getCopy(), mutatableParams[2]->getCopy(), mutatableParams[3]->getCopy(), mutatableParams[4]->getCopy(), mutatableParams[5]->getCopy(), mutatableParams[6]->getCopy(), descendants[0] == NULL ? NULL : descendants[0]->getCopy());
}

void ADSREnvelopeNode::setRenderInfo(float sr, unsigned blockSize, unsigned maxNumFrames, float maxTime) {
    doneRendering();
    sampleRate = sr;
    GPNode::setRenderInfo(sr, blockSize, maxNumFrames, maxTime);
}

void ADSREnvelopeNode::doneRendering() {
    if (preparedToRender) {
        sampleRate = 0;
        free(envelope);
    }
    GPNode::doneRendering();
}

void ADSREnvelopeNode::evaluateBlockPerformance(unsigned firstFrameNumber, unsigned numSamples, float* sampleTimes, unsigned numConstantVariables, float* constantVariables, float* buffer) {
    // if frame number is within the envelope
    if (firstFrameNumber < framesInEnvelope)
        releaseFinished = false;
    else
        releaseFinished = true;

    if (!releaseFinished) {
        // TODO: slight enhancement would be to only evaluate remaining samples
        descendants[0]->evaluateBlockPerformance(firstFrameNumber, numSamples, sampleTimes, numConstantVariables, constantVariables, buffer);
        // if ADSR hasn't finished releasing but will within these n frames
        if (firstFrameNumber + numSamples > framesInEnvelope) {
            for (unsigned i = 0; firstFrameNumber + i < framesInEnvelope; i++) {
                buffer[i] = buffer[i] * envelope[firstFrameNumber + i];
            }
            for (unsigned i = framesInEnvelope - firstFrameNumber; i < numSamples; i++) {
                buffer[i] = 0.0;
            }
            releaseFinished = true;
        }
        // else if ADSR hasn't finished releasing and won't within n
        else {
            for (unsigned i = 0; i < numSamples; i++) {
                buffer[i] = buffer[i] * envelope[firstFrameNumber + i];
            }
        }
    }
    else {
        for (unsigned i = 0; i < numSamples; i++) {
            buffer[i] = 0.0;
        }
    }
}

void ADSREnvelopeNode::updateMutatedParams() {
    GPNode::updateMutatedParams();

	// get minimum value for attack or sustain
    float minAttackHeight = mutatableParams[2]->getCMin();
    if (mutatableParams[5]->getCMin() < minAttackHeight)
    	minAttackHeight = mutatableParams[5]->getCMin();
    
    // get maximum value for attack or sustain
    float maxAttackHeight = mutatableParams[2]->getCMax();
    if (mutatableParams[5]->getCMax() > maxAttackHeight)
    	maxAttackHeight = mutatableParams[5]->getCMax();
    
    // update min/max of envelope ADSR 
    intervalMultiply(&minimum, &maximum, minAttackHeight, maxAttackHeight, descendants[0]->minimum, descendants[0]->maximum);
    
    fillFromParams();
}

/*
    ==============
    CLASS SPECIFIC
    ==============
*/

void ADSREnvelopeNode::fillFromParams() {
	// update class values from mutatable params
    delay = mutatableParams[0]->getCValue();
    delayFrames = delay * sampleRate;

    attack = mutatableParams[1]->getCValue();
    attackFrames = delayFrames + attack * sampleRate;
    attackheight = mutatableParams[2]->getCValue();

    decay = mutatableParams[3]->getCValue();
    decayFrames = attackFrames + decay * sampleRate;

    sustain = mutatableParams[4]->getCValue();
    sustainFrames = decayFrames + sustain * sampleRate;
    sustainheight = mutatableParams[5]->getCValue();

    release = mutatableParams[6]->getCValue();
    releaseFrames = sustainFrames + release * sampleRate;

	// calculate the length of the envelope in frames
    framesInEnvelope = (unsigned) (delay * sampleRate) + (unsigned) (attack * sampleRate) + (unsigned) (decay * sampleRate) + (unsigned) (sustain * sampleRate) + (unsigned) (release * sampleRate);

	// if we are pre-rendering the buffer for efficiency do so here
    envelope = (float*) malloc(sizeof(float) * framesInEnvelope);

    // delay
    unsigned framesFilled = 0;
    for (unsigned i = 0; i < (unsigned) (delay * sampleRate); i++, framesFilled++) {
        envelope[framesFilled] = 0.0;
    }
    // attack
    for (unsigned i = 0; i < (unsigned) (attack * sampleRate); i++, framesFilled++) {
        envelope[framesFilled] = (i / (attack * sampleRate)) * attackheight;
    }
    // decay
    for (unsigned i = 0; i < (unsigned) (decay * sampleRate); i++, framesFilled++) {
        envelope[framesFilled] = attackheight - ((i / (decay * sampleRate)) * (attackheight - sustainheight));
    }
    // sustain
    for (unsigned i = 0; i < (unsigned) (sustain * sampleRate); i++, framesFilled++) {
        envelope[framesFilled] = sustainheight;
    }
    // release
    for (unsigned i = 0; i < (unsigned) (release * sampleRate); i++, framesFilled++) {
        envelope[framesFilled]  = sustainheight - ((i / (release * sampleRate)) * (sustainheight));
    }
}
