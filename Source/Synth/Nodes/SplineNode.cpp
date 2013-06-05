/*
  ==============================================================================

    SplineNode.cpp
    Created: 6 Feb 2013 11:05:21am
    Author:  cdonahue

  ==============================================================================
*/

#include "SplineNode.h"

/*
    ========================
    CONSTRUCTION/DESTRUCTION
    ========================
*/

// points should always have something in it
SplineNode::SplineNode(bool terminal, GPRandom* r, bool ephemeralRandom, GPMutatableParam* splinetype, GPMutatableParam* numpoints, std::vector<GPMutatableParam*>& points, GPNode* signal)
{
    assert(!(splinetype->isMutatable));
    assert(!(numpoints->isMutatable));
    if (ephemeralRandom)
        assert(points.size() == 2);

    terminalSpline = terminal;
    rng = r;
    splineType = splinetype->getDValue();
    numPoints = numpoints->getDValue();

    mutatableParams.push_back(splinetype);
    mutatableParams.push_back(numpoints);

    // randomize the initial points (determines the range if ephemeralRandom == true)
    if (ephemeralRandom) {
        // use points array to determine range of mutatable params
        minimum = points[0]->getCMin();
        maximum = points[0]->getCMax();
        maxSegmentLength = points[1]->getCMax();

        // create the initial value for the spline function
        GPMutatableParam* initialValue = new GPMutatableParam("splinepoint", true, minimum, maximum, maximum);
        initialValue->ephemeralRandom(rng);
        mutatableParams.push_back(initialValue);

        // create the array of spline points
        for (int i = 0; i < numPoints; i++) {
            GPMutatableParam* newSplineLength = new GPMutatableParam("splinelength", true, 0.0f, 0.0f, maxSegmentLength);
            GPMutatableParam* newSplinePoint = new GPMutatableParam("splinepoint", true, minimum, minimum, maximum);
            newSplineLength->ephemeralRandom(rng);
            newSplinePoint->ephemeralRandom(rng);
            mutatableParams.push_back(newSplineLength);
            mutatableParams.push_back(newSplinePoint);
        }
    }
    else {
        for (unsigned i = 0; i < points.size(); i++) {
            mutatableParams.push_back(points[i]);
        }
    }

    if (terminalSpline) {
        arity = 0;
    }
    else {
        arity = 1;
        descendants.push_back(signal);
    }
}

SplineNode::~SplineNode() {
    doneRendering();
}

/*
    =========
    OVERRIDES
    =========
*/

SplineNode* SplineNode::getCopy() {
    // make copies of spline points
    std::vector<GPMutatableParam*> paramCopies(numPoints * 2 + 1);
    for (unsigned i = 2; i < mutatableParams.size(); i++) {
        paramCopies[i] = mutatableParams[i]->getCopy();
    }

    if (terminalSpline) {
        return new SplineNode(terminalSpline, rng, false, mutatableParams[0]->getCopy(), mutatableParams[1]->getCopy(), paramCopies, NULL);
    }
    else {
        return new SplineNode(terminalSpline, rng, false, mutatableParams[0]->getCopy(), mutatableParams[1]->getCopy(), paramCopies, descendants[0] == NULL ? NULL : descendants[0]->getCopy());
    }
}

void SplineNode::setRenderInfo(float sr, unsigned blockSize, unsigned maxNumFrames, float maxTime) {
    doneRendering();
    sampleRate = sr;
    GPNode::setRenderInfo(sr, blockSize, maxNumFrames, maxTime);
}

void SplineNode::doneRendering() {
    if (preparedToRender) {
        sampleRate = 0;
        free(envelope);
    }
    GPNode::doneRendering();
}

void SplineNode::evaluateBlockPerformance(unsigned firstFrameNumber, unsigned numSamples, float* sampleTimes, unsigned numConstantVariables, float* constantVariables, float* buffer) {
    // if frame number is within the envelope
    if (firstFrameNumber < framesInEnvelope)
        releaseFinished = false;
    else
        releaseFinished = true;

    // if this is a terminal node
    if (terminalSpline) {
        if (!releaseFinished) {
            if (firstFrameNumber + numSamples > framesInEnvelope) {
                for (unsigned i = 0; firstFrameNumber + i < framesInEnvelope; i++) {
                    buffer[i] = envelope[firstFrameNumber + i];
                }
                for (unsigned i = framesInEnvelope - firstFrameNumber; i < numSamples; i++) {
                    buffer[i] = 0.0;
                }
                releaseFinished = true;
            }
            else {
                for (unsigned i = 0; i < numSamples; i++) {
                    buffer[i] = envelope[firstFrameNumber + i];
                }
            }
        }
    }
    // if this is not a terminal node
    else {
        if (!releaseFinished) {
            // TODO: slight enhancement would be to only evaluate remaining samples
            descendants[0]->evaluateBlockPerformance(firstFrameNumber, numSamples, sampleTimes, numConstantVariables, constantVariables, buffer);
            // if Spline hasn't finished releasing but will within these n frames
            if (firstFrameNumber + numSamples > framesInEnvelope) {
                for (unsigned i = 0; firstFrameNumber + i < framesInEnvelope; i++) {
                    buffer[i] = buffer[i] * envelope[firstFrameNumber + i];
                }
                for (unsigned i = framesInEnvelope - firstFrameNumber; i < numSamples; i++) {
                    buffer[i] = 0.0;
                }
                releaseFinished = true;
            }
            // else if Spline hasn't finished releasing and won't within n
            else {
                for (unsigned i = 0; i < numSamples; i++) {
                    buffer[i] = buffer[i] * envelope[firstFrameNumber + i];
                }
            }
        }
    }
    // else if Spline has finished releasing for all n frames
    if (releaseFinished) {
        for (unsigned i = 0; i < numSamples; i++) {
            buffer[i] = 0.0;
        }
    }
}

void SplineNode::updateMutatedParams() {
    GPNode::updateMutatedParams();

    // get minimum value for spline envelope
    float minSplineHeight = mutatableParams[2]->getCMin();
    for (int i = 1; i < numPoints; i++) {
        int heightIndex = (i * 2) + 2;
        if (mutatableParams[heightIndex]->getCMin() < minSplineHeight)
            minSplineHeight = mutatableParams[heightIndex]->getCMin();
    }
    
    // get maximum value for attack or sustain
    float maxSplineHeight = mutatableParams[0]->getCMax();
    for (int i = 1; i < numPoints; i++) {
        int heightIndex = (i * 2) + 2;
        if (mutatableParams[heightIndex]->getCMax() > maxSplineHeight)
            maxSplineHeight = mutatableParams[heightIndex]->getCMax();
    }
    
    // update min/max of terminal Spline 
    if (terminalSpline) {
		minimum = minSplineHeight;
		maximum = maxSplineHeight;
	}
	// update descendants and min/max of non-terminal Spline
	else {
        intervalMultiply(&minimum, &maximum, minSplineHeight, maxSplineHeight, descendants[0]->minimum, descendants[0]->maximum);
	}
    
    fillFromParams();
}

void SplineNode::toString(std::stringstream& ss) {
	if (terminalSpline) {
	    ss << "(spline";
	}
	else {
		ss << "(spline*";
	}
    for (unsigned i = 0; i < mutatableParams.size(); i++) {
      ss << " ";
      mutatableParams[i]->toString(ss);
    }
    if (!terminalSpline) {
        ss << " ";
        descendants[0]->toString(ss);
    }
    ss << ")";
}

/*
    ==============
    CLASS SPECIFIC
    ==============
*/

void SplineNode::fillFromParams() {
    if (splineType == 0) {
        unsigned currentFrame = 0;
        unsigned usedPoints = 0;
        float currentLevel = mutatableParams[2]->getCValue();
        while (currentFrame < maxNumFrames && usedPoints < numPoints) {
            float transitionLength = mutatableParams[(usedPoints * 2) + 2 + 1]->getCValue();
            float nextLevel = mutatableParams[(usedPoints * 2) + 2 + 2]->getCValue();
            unsigned currentTransitionFrame = 0;
            unsigned numTransitionFrames = (unsigned) transitionLength * sampleRate;
            float slope = (nextLevel - currentLevel) / float(numTransitionFrames);
            while (currentTransitionFrame < numTransitionFrames && currentFrame < maxNumFrames) {
                envelope[currentFrame] = currentLevel + currentTransitionFrame * slope;
                currentFrame++;
                currentTransitionFrame++;
            }
            usedPoints++;
            currentLevel = nextLevel;
        }
        while (currentFrame < maxNumFrames) {
            envelope[currentFrame] = currentLevel;
            currentFrame++;
        }
    }
    else {
        unsigned currentFrame = 0;
        float currentLevel = mutatableParams[2]->getCValue();
        while (currentFrame < maxNumFrames) {
            envelope[currentFrame] = currentLevel;
            currentFrame++;
        }
    }
}
