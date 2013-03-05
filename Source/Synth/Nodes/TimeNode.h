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
        TimeNode* getCopy();

        double evaluate(double* t, double* v);
        std::string toString();
        void mutate(GPParams* e);

    private:
};

#endif
