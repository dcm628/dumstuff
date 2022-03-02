#ifndef PyroDefinitions_H
#define PyroDefinitions_H

#include "PyroClass.h"
#include <array>

//Igniter outputs - setting up almost the same as valves but no PWM
#define NUM_PYROS 2

Pyro EngineIgniter0{0, 2, Solo, 11, 10000000, 5000000};
Pyro EngineIgniter1{1, 2, Solo, 12, 10000000, 5000000};


// ADD PYROS TO THIS VALVE ARRAY IN THE FORM: &PYRO

std::array<Pyro*, NUM_PYROS> pyroArray{&EngineIgniter0, &EngineIgniter1};

#endif