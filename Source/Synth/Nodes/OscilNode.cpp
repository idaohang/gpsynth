/*
  ==============================================================================

    OscilNode.cpp
    Created: 6 Feb 2013 11:05:21am
    Author:  cdonahue

  ==============================================================================
*/

#include "OscilNode.h"

/*
    ==============
    PUBLIC METHODS
    ==============
*/

OscilNode::OscilNode(int p, int vn) {
    partial = p;
    variableNum = vn;
    left = NULL;
    right = NULL;
    parent = NULL;
    w = 2.0 * partial * M_PI;
    isBinary = false;
    isTerminal = true;
}

OscilNode::~OscilNode() {
}

OscilNode* OscilNode::getCopy() {
    return new OscilNode(partial, variableNum);
}

double OscilNode::evaluate(double* t, double* v) {
    return sin(w*(*t)*(v[variableNum]));
}

void OscilNode::evaluateBlock(double* t, unsigned nv, double* v, unsigned n, float* buffer) {
    double* currentIndex = v + variableNum;
	for (int i = 0; i < n; i++) {
        buffer[i] = sin(w*(t[i])*(*currentIndex));
        currentIndex += nv;
    }
}

std::string OscilNode::toString() {
    char buffer[30];
    snprintf(buffer, 30, "(osc p%d v%d)", partial, variableNum);
    return std::string(buffer);
}

void OscilNode::mutate(GPParams* p) {
    if (p->rng->random() < p->oscilNodePartialChance) {
        partial = (int) (p->rng->random() * p->oscilNodeNumPartials);
        w = 2.0 * partial * M_PI;
    }
    else {
        variableNum = p->rng->random(p->numVariables);
    }
}
