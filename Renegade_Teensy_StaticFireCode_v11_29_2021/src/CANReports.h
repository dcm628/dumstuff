#ifndef CANREPORTS_H
#define CANREPORTS_H

#include <Arduino.h>
#include <FlexCAN.h>
#include <array>

#include "CANWrite.h"
#include "StateList.h"
#include "ControlFunctions.h"
#include <vector>


// General Level State Report - covers overall state of whole node
void CAN2PropSystemStateReport(FlexCAN& CANbus, State& currentState, Command& currentCommand, const std::array<Valve*, NUM_VALVES>& valveArray, const std::array<Pyro*, NUM_PYROS>& pyroArray, const std::array<ValveEnable*, NUM_VALVEENABLE>& valveEnableArray, bool & haltFlag, uint8_t nodeID)
{
// , int8_t autosequencetimer
//Serial.println("Do I even run the State Report?");
// build message
    static CAN_message_t msgOut;
    msgOut.ext = 0;
    msgOut.id = nodeID;  //Make this more specific later

    // CAN BYTE - System state and Valve Safety Enable States
    int8_t currentStateEnumToInt = static_cast<int8_t>(currentState);
    //Serial.print("static cast of StateEnumToInt: ");
    //Serial.println(currentStateEnumToInt);
    bool ValveSafetyEnableStateArray[3] ={};
    uint8_t ShiftedValveSafetyEnableStateArray = 0;

    // iterate through valveEnable array and get states for state report
    for(auto valveEnable : valveEnableArray)
    {
        if (valveEnable->getValveEnableNodeID() == nodeID)
        {    
            for (size_t i = 0; i < 3; i++) //I need to add something for sizes less than 3 so it doesn't bug out
            {
            bool ValveEnableEnumToBool = static_cast<bool>(valveEnable->getState());
            ValveSafetyEnableStateArray[i] = ValveEnableEnumToBool;
            }
        }        
    }

    //bitshifting array
    for (size_t i = 0; i < 3; i++)
    {
    uint8_t bitShiftNumber = 4 + i;
    uint8_t loopyboi = (ValveSafetyEnableStateArray[i])<<bitShiftNumber;
    ShiftedValveSafetyEnableStateArray = ShiftedValveSafetyEnableStateArray + loopyboi;
    }

    //valveEnableArray;
    msgOut.buf[0] = currentStateEnumToInt + ShiftedValveSafetyEnableStateArray;
    //msgOut.buf[0] = 111;

    //Valve State information bytes
    uint8_t canByte = 1;    //starts the valve state bytes skipping 1 byte(s) first
    while (canByte < 7)     //to limit overflow of CAN2.0 max message bytes
    {
    
        // iterate through valve array 
        for(auto valve : valveArray)
        {
    
            if (valve->getValveNodeID() == nodeID)
            {
            uint8_t valveID = static_cast<uint8_t>(valve->getValveID());    
            uint8_t ValveStateEnumToInt = static_cast<uint8_t>(valve->getState());
            uint8_t ShiftedValveStateEnumToInt = (ValveStateEnumToInt<<5);
            
            msgOut.buf[canByte] = valveID + ShiftedValveStateEnumToInt;
            Serial.println("ValveID: ");
            Serial.print(valveID);
            Serial.print( ": ValveState: ");
            Serial.print(ValveStateEnumToInt);
            Serial.print(": ");
            Serial.print(ShiftedValveStateEnumToInt);
            Serial.print(": ");
            Serial.println(canByte);
            canByte++;
            }
        
        }

    }
    
    // write message to bus
    Serial.print("ID: ");
    Serial.print(msgOut.id);
    Serial.print(": ");
    for (size_t ii = 0; ii < 8; ii++)
    {
        Serial.print(msgOut.buf[ii]);
        Serial.print(": ");
    }
    Serial.println();
    msgOut.len = 8;
    CANbus.write(msgOut);
    {
        // add write error handling here, for now it does nothing
    }

//return ;
}

void CAN2AutosequenceTimerReport(FlexCAN& CANbus, const std::array<AutoSequence*, NUM_AUTOSEQUENCES>& autoSequenceArray, bool & haltFlag, int nodeID)
{
// build message
    static CAN_message_t msgOut;
    msgOut.ext = 0;
    //change ID format to be better and match my updated plan
    for(auto autoSequence : autoSequenceArray)
    {
        msgOut.id = nodeID + 16;  // with 16 possible nodes in ID format this makes the CAN ID possible go up to 31, lowest sensor ID in current format is 50.

        int32_t autosequenceTimer = autoSequence->getCurrentCountdown();
        uint8_t autosequenceTimerStateEnumToInt = static_cast<uint8_t>(autoSequence->getAutoSequenceState());


        //int8_t autosequenceTimerState = autoSequence->getAutoSequenceState;
        msgOut.buf[0] = autosequenceTimerStateEnumToInt;

        msgOut.buf[1] = autosequenceTimer;
        msgOut.buf[2] = (autosequenceTimer >> 8);
        msgOut.buf[3] = (autosequenceTimer >> 16);
        msgOut.buf[4] = (autosequenceTimer >> 24);
    
    
        // write message to bus
        CANbus.write(msgOut);
        {
            // add write error handling here, for now it does nothing
        }
    
    }
}


/* bool CAN2ValveStateReport()
{
    int8_t byte0 = 0;




} */



////// CAN FD Upgrade Versions //////
// TBD //

/* bool CANFDPropSystemStateReport()
{

} */


/* bool CANFDValveStateReport()
{

} */

#endif