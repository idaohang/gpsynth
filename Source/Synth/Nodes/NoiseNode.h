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
        NoiseNode* getCopy();

        void evaluateBlock(double* t, unsigned nv, double* v, double* min, double* max, unsigned n, float* buffer);
        std::string toString();
        void updateMutatedParams();

    private:
        GPRandom* rng;
};

#endif
