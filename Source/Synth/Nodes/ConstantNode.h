/*
  ==============================================================================

    ConstantNode.h
    Created: 6 Feb 2013 11:06:13am
    Author:  cdonahue

  ==============================================================================
*/

#ifndef CONSTANTNODE_H
#define CONSTANTNODE_H

#include "../GPNode.h"

class ConstantNode: public GPNode {
    public:
        ConstantNode(double v);
        ~ConstantNode();
        ConstantNode* getCopy();

        double evaluate(double* t, double* v);
        void evaluateBlock(double* t, double** v, unsigned n, float* buffer);
        std::string toString();
        void mutate(GPParams* e);

    private:
        double value;
};

#endif
