/*
  ==============================================================================

    ModOscilNode.h
    Created: 6 Feb 2013 11:05:21am
    Author:  cdonahue

  ==============================================================================
*/

#ifndef MODOSCILNODE_H
#define MODOSCILNODE_H

#include <cmath>
#include "../GPNode.h"

class ModOscilNode: public GPNode {
    public:
        ModOscilNode(GPNode* freq, GPNode* phase);
        ~ModOscilNode();
        ModOscilNode* getCopy();

        void evaluateBlock(unsigned fn, double* t, unsigned nv, double* v, double* min, double* max, unsigned n, float* buffer);
        std::string toString();
        void updateMutatedParams();

    private:
        //int partial;
        double w;
};

#endif