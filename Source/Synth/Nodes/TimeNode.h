/*
  ==============================================================================

    TimeNode.h
    Created: 6 Feb 2013 11:06:13am
    Author:  cdonahue

  ==============================================================================
*/

#ifndef TIMENODE_H
#define TIMENODE_H

#include "../GPNode.h"

class TimeNode: public GPNode {
public:
    TimeNode();
    ~TimeNode();

	// overrides
    TimeNode* getCopy();
	void evaluateBlockPerformance(unsigned firstFrameNumber, unsigned numSamples, float* sampleTimes, unsigned numConstantVariables, float* constantVariables, float* buffer);
    void toString(std::stringstream& ss);

    // optional overrides
	void setRenderInfo(float sr, unsigned blockSize, float maxTime);
	void doneRendering();

private:
};

#endif
