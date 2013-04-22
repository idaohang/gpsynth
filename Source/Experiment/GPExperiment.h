/*
  ==============================================================================

    GPExperiment.h
    Created: 6 Feb 2013 11:05:21am
    Author:  cdonahue

  ==============================================================================
*/

#ifndef GPEXPERIMENT_H
#define GPEXPERIMENT_H

#include "../Common/GPFunctions.h"
#include "../Common/GPParams.h"
#include "../Common/GPHelpers.h"

#include "../Synth/GPSynth.h"
#include "../Synth/Nodes/ADSRNode.h"
#include "../Synth/Nodes/ConstantNode.h"
#include "../Synth/Nodes/TimeNode.h"
#include "../Synth/Nodes/VariableNode.h"
#include "../Synth/Nodes/FunctionNode.h"
#include "../Synth/Nodes/OscilNode.h"
//#include "../Synth/Nodes/FilterNode.h"
#include "../Synth/Nodes/NoiseNode.h"

#include "../Dependencies/kissfft/kiss_fftr.h"
#include "../../JuceLibraryCode/JuceHeader.h"

#include <limits>
#include <fstream>

class GPExperiment {
public:
    // CONSTUCTION
    GPExperiment(GPRandom* r, unsigned s, String target, String path, GPParams* p, double* constants, bool* rq);
    ~GPExperiment();

    // EVOLUTION CONTROL
    GPNetwork* evolve();

private:
    // EXPERIMENT PARAMETERS
    GPParams* params;
    unsigned seed;
    String savePath;
    float fitnessThreshold;
    int numGenerations;
    bool lowerFitnessIsBetter;

    // TARGET DATA CONTAINERS
    // metadata
    double targetSampleRate;
    double targetNyquist;
    unsigned numTargetFrames;
    // time domain
    float* targetFrames;
    float* targetEnvelope;
    // freq domain
    kiss_fft_cpx* targetSpectrum;
    double* targetSpectrumMagnitudes;
    double* targetSpectrumPhases;
    // fitness function analysis
    float* analysisWindow;
    double* weightMatrix;
    double* binOvershootingPenalty;
    double* binUndershootingPenalty;

    // EVALUATION DATA
    double* sampleTimes;
    double* specialValues;
    // TODO: replace specialValues with specialValuesByTime
    double* specialValuesByFrame;
    unsigned numSpecialValues;

    // EXPERIMENT STATE
    float minFitnessAchieved;
    int numEvaluatedGenerations;
    bool* requestedQuit;

    // SYNTH
    GPSynth* synth;

    // FILL EVALUATION BUFFERS
    void fillEvaluationBuffers(double* constantSpecialValues, double* variableSpecialValues, unsigned numConstantSpecialValues, unsigned numVariableSpecialValues);

    // WAV INTERFACE
    ScopedPointer<WavAudioFormat> wavFormat;
    unsigned wavFileBufferSize;
    void getWavFileInfo(String path, unsigned* numFrames, double* sampleRate);
    void loadWavFile(String path, unsigned n, float* buffer);
    void saveWavFile(String path, String metadata, unsigned numFrames, double sampleRate, float* data);
    void saveTextFile(String path, String text);

    // FITNESS FUNCTION
    void renderIndividualByBlock(GPNetwork* candidate, int64 numSamples, unsigned n, float* buffer);
    double compareToTarget(unsigned type, float* candidateFrames);

    // FOURIER TRANSFORM
    float dBRef;
    unsigned calculateFftBufferSize(unsigned numFrames, unsigned n, unsigned o);
    void FftReal(unsigned numFrames, const float* input, unsigned n, unsigned overlap, const float* window, kiss_fft_cpx* out, bool dB, float dBref, double* magnitude, double* phase);

    // WAVEFORM OPERATIONS
    void window(const char* type, unsigned n, float* windowBuffer);
    void findMovingAverage(unsigned type, unsigned n, const double* buffer, double* movingaverage, unsigned R, double* maxdeviationabove, double* maxdeviationbelow);
    void applyWindow(unsigned n, kiss_fft_scalar* buffer, const float* window);
    void applyEnvelope(unsigned n, float* buffer, const float* envelope);
    void applyEnvelope(unsigned n, const float* buffer, const float* envelope, float* windowedBuffer);
    void followEnvelope(unsigned n, float* buffer, float* envelope, double attack_in_ms, double release_in_ms, double samplerate);
    void findEnvelope(bool ignoreZeroes, unsigned n, float* wav, float* env);

    // GRAPH HELPERS
    void fillFrequencyAxisBuffer(unsigned fftSize, double sr, float* buffer);
    String floatBuffersToGraphText(String options, String xlab, String ylab, bool indexAsX, unsigned n, float* x, float* y);
};

#endif
