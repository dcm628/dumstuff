#ifndef CANREPORTS_H
#define CANREPORTS_H

#include <Arduino.h>
#include <FlexCAN.h>
#include <array>

#include "CANWrite.h"
#include "StateList.h"
#include "ControlFunctions.h"
#include <vector>

uint8_t engineNodeValveNum = 4;
uint8_t propNodeValveNum = 6;
uint8_t engineNodePyroNum = 2;
uint8_t propNodePyroNum = 0;
uint8_t vavleArrayCount;
uint8_t pyroArrayCount;

// General Level State Report - covers overall state of whole node
void CAN2PropSystemStateReport(FlexCAN& CANbus, State& currentState, Command& currentCommand, const std::array<Valve*, NUM_VALVES>& valveArray, const std::array<Pyro*, NUM_PYROS>& pyroArray, const std::array<ValveEnable*, NUM_VALVEENABLE>& valveEnableArray, bool & haltFlag, uint8_t nodeID)
{
//Hardcoded array iterator sizes because I'm not smart enough to fix the auto arrays yet
if (nodeID == 2)
{
    vavleArrayCount = engineNodeValveNum;
    pyroArrayCount = engineNodePyroNum;
}
else if (nodeID == 3)
{
    vavleArrayCount = propNodeValveNum;
    pyroArrayCount = propNodePyroNum;
}
else
{
    vavleArrayCount = 7;
    pyroArrayCount = 0;
}


// build message
    static CAN_message_t msgOut;
    msgOut.ext = 0;
    msgOut.id = nodeID;  //Make this more specific later

    // CAN BYTE - System state and Valve Safety Enable States
    int8_t currentStateEnumToInt = static_cast<int8_t>(currentState);
    //Serial.print("static cast of StateEnumToInt: ");
    //Serial.println(currentStateEnumToInt);
    bool ValveSafetyEnableStateArray[4] ={};
    uint8_t ShiftedValveSafetyEnableStateArray = 0;
    uint8_t EnableArrayBit = 0;    //for the valve enable iterator

    // iterate through valveEnable array and get states for state report
    for(auto valveEnable : valveEnableArray)
    {
        if (valveEnable->getValveEnableNodeID() == nodeID)
        {    
            //for (size_t i = 0; i < 3; i++) //I need to add something for sizes less than 3 so it doesn't bug out
            //{
            bool ValveEnableEnumToBool = static_cast<bool>(valveEnable->getState());
            ValveSafetyEnableStateArray[EnableArrayBit] = ValveEnableEnumToBool;
            EnableArrayBit++;
            //}
        }        
    }

    //bitshifting array
    for (size_t i = 0; i < 4; i++)
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
         //to limit overflow of CAN2.0 max message bytes
    
    //while (canByte <=7)
    //{
        //while (canByte <=vavleArrayCount) //HARDCODING number of valves on engine node so it doesn't loop
        //{
        
/*         // iterate through valve array - attempting to do it different way
        for (valveArray)
        {

        } */
        
        
        // iterate through valve array 
        for(auto valve : valveArray)
        {
            
                if (valve->getValveNodeID() == nodeID)
                {
                uint8_t valveID = static_cast<uint8_t>(valve->getValveID());    
                uint8_t ValveStateEnumToInt = static_cast<uint8_t>(valve->getState());
                uint8_t ShiftedValveStateEnumToInt = (ValveStateEnumToInt<<5);
                
                msgOut.buf[canByte] = valveID + ShiftedValveStateEnumToInt;
                Serial.print("ValveID: ");
                Serial.print(valveID);
                Serial.print( ": ValveState: ");
                Serial.print(ValveStateEnumToInt);
                Serial.print(": ");
                //Serial.print(ShiftedValveStateEnumToInt);
                //Serial.print("CANbyte: ");
                Serial.println(canByte);
                canByte++;
                }
            //canByte++;
            
        }
        //}
        //while (canByte <=pyroArrayCount) //HARDCODING number of valves on engine node so it doesn't loop
        //{
        for(auto pyro : pyroArray)
        {
                if (pyro->getPyroNodeID() == nodeID)
                {
                uint8_t pyroID = static_cast<uint8_t>(pyro->getPyroID());    
                uint8_t PyroStateEnumToInt = static_cast<uint8_t>(pyro->getState());
                uint8_t ShiftedPyroStateEnumToInt = (PyroStateEnumToInt<<5);
                
                msgOut.buf[canByte] = pyroID + ShiftedPyroStateEnumToInt;
                Serial.print("PyroID: ");
                Serial.print(pyroID);
                Serial.print( ": PyroState: ");
                Serial.print(PyroStateEnumToInt);
                Serial.print(": ");
                Serial.println(canByte);
                canByte++;
                }
        }
        //}
    //}
    
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
    canByte = 1;
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
        msgOut.len = 8;
        int64_t autosequenceTimer = autoSequence->getCurrentCountdown();
        uint8_t autosequenceTimerStateEnumToInt = static_cast<uint8_t>(autoSequence->getAutoSequenceState());

        Serial.print("Autosequence: State : ");
        Serial.print(autosequenceTimerStateEnumToInt);
        Serial.print(" Timer : ");
        Serial.print(autosequenceTimer);
        Serial.println();

        msgOut.buf[0] = autosequenceTimerStateEnumToInt;
        msgOut.buf[1] = autosequenceTimer;
        msgOut.buf[2] = (autosequenceTimer >> 8);
        msgOut.buf[3] = (autosequenceTimer >> 16);
        msgOut.buf[4] = (autosequenceTimer >> 24);
        msgOut.buf[5] = (autosequenceTimer >> 32);
        msgOut.buf[6] = (autosequenceTimer >> 40);
        msgOut.buf[7] = (autosequenceTimer >> 48);
        
        // write message to bus
        CANbus.write(msgOut);
        {
            // add write error handling here, for now it does nothing
        }
    
    }
}


/* bool CAN2ValveStateReport()
{
    




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