/*
  ==============================================================================

    GPPrimitives.h
    Author:  cdonahue

  ==============================================================================
*/

#ifndef GPPRIMITIVES_H
#define GPPRIMITIVES_H

// Effect includes

// Envelope includes
#include "Nodes/Envelope/ADSREnvelopeNode.h"
#include "Nodes/Envelope/GainNode.h"
#include "Nodes/Envelope/LFOEnvelopeNode.h"
#include "Nodes/Envelope/SplineEnvelopeNode.h"

// Function includes
#include "Nodes/Function/FunctionNode.h"
#include "Nodes/Function/AddNode.h"
#include "Nodes/Function/SubtractNode.h"
#include "Nodes/Function/MultiplyNode.h"
#include "Nodes/Function/SineNode.h"
#include "Nodes/Function/CosineNode.h"

// Logic includes
#include "Nodes/Logic/MixerNode.h"
#include "Nodes/Logic/SwitchNode.h"

// Modulation includes
#include "Nodes/Modulation/AMNode.h"
#include "Nodes/Modulation/PMNode.h"
//#include "Nodes/Modulation/FMNode.h"
#include "Nodes/Modulation/FreqOscillator/SinFreqOscNode.h"
#include "Nodes/Modulation/FreqOscillator/SawFreqOscNode.h"
#include "Nodes/Modulation/FreqOscillator/SquareFreqOscNode.h"
#include "Nodes/Modulation/FreqOscillator/TriangleFreqOscNode.h"

// Terminal includes
#include "Nodes/Terminal/Oscillator/SinOscNode.h"
#include "Nodes/Terminal/Oscillator/SawOscNode.h"
#include "Nodes/Terminal/Oscillator/SquareOscNode.h"
#include "Nodes/Terminal/Oscillator/TriangleOscNode.h"
#include "Nodes/Terminal/ADSRTerminalNode.h"
#include "Nodes/Terminal/ConstantNode.h"
#include "Nodes/Terminal/LFOTerminalNode.h"
#include "Nodes/Terminal/NoiseNode.h"
#include "Nodes/Terminal/SilenceNode.h"
#include "Nodes/Terminal/SplineTerminalNode.h"
#include "Nodes/Terminal/TimeNode.h"
#include "Nodes/Terminal/VariableNode.h"
#include "Nodes/Terminal/WaveTableNode.h"

#endif
