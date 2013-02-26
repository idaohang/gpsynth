/*
  ==============================================================================

    OscilNode.h
    Created: 6 Feb 2013 11:05:21am
    Author:  cdonahue

  ==============================================================================
*/

#ifndef OSCILNODE_H
#define OSCILNODE_H

#include <cmath>
#include "../GPNode.h"

class OscilNode: public GPNode {
    public:
        OscilNode(int p, int vn, GPNode* l, GPNode* r);
        ~OscilNode();
        OscilNode* getCopy();

        double evaluate(double* t, double* v);
        std::string toString();
        bool equals(GPNode* other);
        void mutate(GPNodeParams* e);

    private:
        int partial;
        int variableNum;
        double w;
};

#endif
