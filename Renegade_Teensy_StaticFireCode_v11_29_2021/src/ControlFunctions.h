#ifndef CONTROLFUNCTIONS_H
#define CONTROLFUNCTIONS_H

#include "StateList.h"
#include "ControlCommands.h"
#include "ValveClass.h"
#include "PyroClass.h"
#include <array>

#define NUM_VALVES 10
#define NUM_PYROS 2

// checks the state that was set at start-up and issues the appropriate command as current command
void startupStateCheck(const State& currentState, Command& currentCommand);

// takes the current command and executes the necessary instructions
void commandExecute(State& currentState, Command& currentCommand, const std::array<Valve*, NUM_VALVES>& valveArray, const std::array<Pyro*, NUM_PYROS>& pyroArray);

// Used with Abort to override control of main valves and engine igniters
bool haltFlag; //creates halt flag
void haltFlagCheck(const std::array<Valve*, NUM_VALVES>& valveArray, const std::array<Pyro*, NUM_PYROS>& pyroArray, bool haltFlag);



#endif