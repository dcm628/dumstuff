#ifndef CANREPORTS_H
#define CANREPORTS_H

#include <Arduino.h>
#include <FlexCAN.h>
#include <array>
#include "ValveClass.h"
#include "ControlCommands.h"
#include "CANWrite.h"
#include "StateList.h"
#include "pinList.h"

#include "ControlFunctions.h"
//#include "AutoSequence.h"

// 
void CAN2PropSystemStateReport(FlexCAN& CANbus, State& currentState, Command& currentCommand, const std::array<Valve*, NUM_VALVES>& valveArray, const std::array<Pyro*, NUM_PYROS>& pyroArray, const std::array<ValveEnable*, NUM_VALVEENABLE>& valveEnableArray, bool & haltFlag, uint8_t nodeID)

//bool CAN2AutosequenceTimerReport(FlexCAN& CANbus, AutoSequence& autoSequence, bool & haltFlag, int nodeID, int8_t autosequencetimer, uint8_t autosequenceTimerState)



#endif