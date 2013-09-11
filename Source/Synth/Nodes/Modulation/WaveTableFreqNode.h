#ifndef WAVETABLEFREQNODE_H
#define WAVETABLEFREQNODE_H

#include "../../GPNode.h"
#include "../../../Dependencies/WaveTableOsc/WaveTableOsc.h"

class WaveTableFreqNode: public GPNode {
public:
    // overrides
    void setRenderInfo(float sr, unsigned blockSize, unsigned maxFrameNumber, float maxTime);
    void doneRendering();
    void evaluateBlockPerformance(unsigned firstFrameNumber, unsigned numSamples, float* sampleTimes, unsigned numConstantVariables, float* constantVariables, float* buffer);
	void updateMutatedParams();

	// WaveTable helpers
	void fft(int N, double* ar, double* ai);
	float makeAddWaveTable(int len, double* ar, double* ai, double scale, double topFreq);

	// GPNode subclass overrides
    virtual WaveTableFreqNode* getCopy() = 0;

	// WaveTable subclass overrides
	virtual void makeAddAllWaveTables(double sampleRate, unsigned overSamp, unsigned constantRatioLimit, double baseFreq, double topFreq) = 0;
    virtual void defineHarmonics(int len, int numHarmonics, double* ar, double* ai) = 0;

    // member variables
    int variableNum;
    float sampleRate;
    float nyquistFreq;
    float phase;
    float freq_m;
    float freq_b;
    WaveTableOsc* osc;
private:
};

#endif
