/*
  ==============================================================================

    VariableNode.cpp
    Created: 6 Feb 2013 11:06:13am
    Author:  cdonahue

  ==============================================================================
*/

#include "VariableNode.h"

VariableNode::VariableNode(int vn) {
    variableNum = vn;

    isBinary = false;
    isTerminal = true;
}

VariableNode::~VariableNode() {
}

VariableNode* VariableNode::getCopy() {
    return new VariableNode(variableNum);
}

double VariableNode::evaluate(double* t, double* v) {
    return v[variableNum];
}

void VariableNode::evaluateBlock(double* t, unsigned nv, double* v, unsigned n, float* buffer) {
    double* currentIndex = v + variableNum;
    for (int i = 0; i < n; i++) {
        buffer[i] = *currentIndex;
        currentIndex += nv;
    }
}

std::string VariableNode::toString() {
    char buffer[10];
    snprintf(buffer, 10, "(v%d)", variableNum);
    return std::string(buffer);
}

void VariableNode::updateMutatedParams() {
}
