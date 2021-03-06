#ifndef VALVECLASS_H
#define VALVECLASS_H

#include <Arduino.h>
#include "ValveStates.h"

// This class defines the Valve Object that will be used to represent and actuate the valves
// Run begin to set the pins

enum ValveType
{
    NormalClosed,
    NormalOpen,
};



class Valve
{

    private:
        const uint32_t valveID;                          // Valve ID number 
        const uint32_t valveNodeID;                      // NodeID the valve is controlled by
        const ValveType valveType;                  // sets the valve type, either normal closed or normal open
        const uint8_t pin;                              // Valve PWM pin for actuation
        const uint32_t fullDutyTime;                // Time PWM needs to be at full duty for actuation, in MICROS
        const int32_t fireSequenceTime;             // Time to wait until actuation after fire command given, in MICROS
        ValveState state;                           // Tracks the valve state
        elapsedMicros timer;                        // timer for the valve, used for changing duty cycles, in MICROS
        const uint8_t fullDuty{255};                // full duty cycle for servo initial actuation
        const uint8_t holdDuty{};                   // partial duty cycle to hold valve in actuated state
        bool fireCommandBool;                       // Whether this valve is on the Ignition AutoSequence for FireCommand timer check
        bool nodeIDCheck;                           // Whether this object should operate on this node

    public:
    
    // constructor, define the valve ID here, and the pin that controls the valve, setFireDelay is only parameter that can be left blank
        Valve(uint32_t setValveID, uint32_t setValveNodeID, ValveType setValveType, uint8_t setPin, uint32_t setFullDutyTime, bool setFireCommandBool, int32_t setFireSequenceTime = 2147483648, uint8_t setHoldDuty = 64, bool setNodeIDCheck = false);

    // a start up method, to set pins from within setup()
        void begin();

    // access functions defined in place

    // get functions, return the current value of that variable
        uint32_t getValveID(){return valveID;}
        uint32_t getValveNodeID(){return valveNodeID;}
        ValveType getValveType(){return valveType;}
        uint8_t getPin(){return pin;}
        uint32_t getFullDutyTime(){return fullDutyTime;}
        uint8_t getHoldDuty(){return holdDuty;}
        int32_t getFireSequenceTime(){return fireSequenceTime;}
        ValveState getState(){return state;}
        uint32_t getTimer(){return timer;}
        bool getFireCommandBool(){return fireCommandBool;}
        bool getNodeIDCheck(){return nodeIDCheck;}

    // set functions, allows the setting of a variable
        void setState(ValveState newState) {state = newState; timer = 0;} //every time a state is set, the timer should reset

    // set the Fire Sequence bool function
        void setFireCommandBool(bool updatedFireCommandBool) {fireCommandBool = updatedFireCommandBool;}

    // set the Node ID Check bool function
        void setNodeIDCheck(bool updatedNodeIDCheck) {nodeIDCheck = updatedNodeIDCheck;}

    // functions with executables defined in ValveClasses.cpp
        void resetTimer();              // resets timer to zero, timer increments automatically in microseconds

    // ----- THIS METHOD TO BE RUN EVERY LOOP ------
    // stateOperations will check the current state of the valve and perform any actions that need to be performed
    // for example, if the valve is commanded to open, this needs to be run so that the valve can start opening
    // and it needs to be run every loop so that once enough time has pass the 
        void stateOperations();



};

class ValveEnable
{
    private:
        const uint32_t valveEnableID;                          // Valve Enable ID number 
        const uint8_t valveEnableNodeID;
        const uint32_t valveEnablePin;
        ValveEnableState state;
        bool nodeIDCheck;                           // Whether this object should operate on this node

    public:
        
    // constructor
        ValveEnable (uint32_t setValveEnableID,  uint32_t setValveEnablePin, uint8_t setValveEnableNodeID, bool setNodeIDCheck = false);


    // a start up method, to set pins from within setup()
        void begin();


    // get functions, return the current value of that variable
        uint32_t getValveEnableID(){return valveEnableID;}
        uint32_t getValveEnableNodeID(){return valveEnableNodeID;}
        uint8_t getValveEnablePin(){return valveEnablePin;}
        ValveEnableState getState(){return state;}   
        bool getNodeIDCheck(){return nodeIDCheck;}

    // set functions, allows the setting of a variable
        void setState(ValveEnableState newState) {state = newState;}
    
    // set the Node ID Check bool function
        void setNodeIDCheck(bool updatedNodeIDCheck) {nodeIDCheck = updatedNodeIDCheck;}
    
    // samesies as above
        void stateOperations();

};



#endif