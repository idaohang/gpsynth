/*
  ==============================================================================

    NoiseNode.h
    Created: 6 Feb 2013 11:05:21am
    Author:  cdonahue

  ==============================================================================
*/

#ifndef NOISENODE_H
#define NOISENODE_H

#include "../GPNode.h"
#include <list>

class NoiseNode: public GPNode {
public:
    NoiseNode(GPRandom* r);
    ~NoiseNode();

	// overrides
    NoiseNode* getCopy();
	void setRenderInfo(float sr, unsigned blockSize, float maxTime);
	void prepareToPlay();
    void evaluateBlock(unsigned fn, double* t, unsigned nv, double* v, double* min, double* max, unsigned n, float* buffer);
	void evaluateBlockPerformance(unsigned firstFrameNumber, unsigned numSamples, float* sampleTimes, unsigned numConstantVariables, float* constantVariables, float* buffer);
	void updateMutatedParams();
    void toString(bool printRange, std::stringstream& ss);

	// class specific
	void whitenoise(float* _fpDstBuffer, unsigned int _uiBufferSize, float _fLevel );

private:
    GPRandom* rng;
    float g_fScale;
    int g_x1;
    int g_x2;

    // whitenoise stuff
};

#endif
