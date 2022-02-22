#ifndef OPERATIONFUNCTIONS_H
#define OPERATIONFUNCTIONS_H

#define BITFLAG_SIZE 16

#include "ValveClass.h"
#include "PyroClass.h"
#include "SensorClass.h"
#include <array>
#include <bitset>
#include <FlexCAN.h>
#include <ADC.h>



// This contains some of the functions to be used during operations they are templates, and so defined in the header. BEWARE

//  CALL THIS FUNCTION EVERY LOOP 
    // This function takes the array of pointers that point to the valve objects, and then calls the .stateOperations() method for each valve
    // Make sure valveArray is an array of pointers, as defined in ValveDefinitions.h
template <typename T, std::size_t size>
void valveTasks(const std::array<T, size>& valveArray, uint8_t& nodeID)
{
    // iterate through valve array and run the stateOperations method
    for(auto valve : valveArray)
    {
    
        if (valve->getValveNodeID() == nodeID)
            {
            valve->stateOperations();
            //Serial.print("LoopRan");
            }
    }
}

template <typename T, std::size_t size>
void valveEnableTasks(const std::array<T, size>& valveEnableArray, uint8_t& nodeID)
{
    // iterate through valve array and run the stateOperations method
    for(auto valveEnable : valveEnableArray)
    {
        if (valveEnable->getValveEnableNodeID() == nodeID)
/*             Serial.println("valve node ID");
            Serial.println(valveEnable->getValveEnableNodeID());
            Serial.print("nodeID");
            Serial.print(nodeID); */
            {
            valveEnable->stateOperations();
            //Serial.print("LoopRan");
            }        
    }
}

template <typename T, std::size_t size>
void pyroTasks(const std::array<T, size>& pyroArray, uint8_t& nodeID)
{
    // iterate through valve array and run the stateOperations method
    for(auto pyro : pyroArray)
    {
        
    if (pyro->getPyroNodeID() == nodeID)
            {
            pyro->stateOperations();
            //Serial.print("LoopRan");
            }
    }
}


// CALL THIS FUNCTION ONCE IN SETUP, THIS SETS THE VALVE PINMODES
    // make sure to pass this function valveArray, as defined in ValveDefinitions.h
template <typename T, std::size_t size>
void valveSetUp(const std::array<T, size>& valveArray)
{
    // iterate through valve array and run the stateOperations method
    for(auto valve : valveArray)
    {
        valve->begin();
        //Serial.print("LoopRan");
    }
}

template <typename T, std::size_t size>
void valveEnableSetUp(const std::array<T, size>& valveEnableArray)
{
    // iterate through valve array and run the stateOperations method
    for(auto valveEnable : valveEnableArray)
    {
        valveEnable->begin();
        //Serial.print("LoopRan");
    }
}

template <typename T, std::size_t size>
void pyroSetUp(const std::array<T, size>& pyroArray)
{
    // iterate through valve array and run the stateOperations method
    for(auto pyro : pyroArray)
    {
        pyro->begin();
        //Serial.print("LoopRan");
    }
}

/* // This function outputs a bitset contianing flags about the valve status, to be sent back to the console via CAN
template <typename T, std::size_t size>
std::bitset<BITFLAG_SIZE> setValveFlags(const std::array<T, size>& valveArray)
{
    std::bitset<BITFLAG_SIZE> valveStatus{0b0000'0000'0000'0000};


    for (auto valve : valveArray)
    {
        ValveState valveState{valve->getState()};
        if((valveState == ValveState::Open) || (valveState == ValveState::OpenProcess) || (valveState == ValveState::CloseCommanded)) // these two states count as open, everything else is closed
        {
            valveStatus.set(valve->getValveID());
            //Serial.println("Open");
        }
        else
        {
            //Serial.println("Closed");
        }

    }
    
    return valveStatus;
} */

// this runs the begin method for each sensor
template <std::size_t size>
void sensorSetUp(const std::array<SENSOR*, size>& sensorArray, uint8_t& nodeID)
{
    
    for(auto sensor : sensorArray)
    {
        sensor->begin();
    }
}

// This function reads all the sensor values
template <std::size_t size>
void readSensors(const std::array<SENSOR*, size>& sensorArray, ADC* adc, uint8_t& nodeID)
{
    if (sensorArray[0]->getSensorNodeID() == nodeID)
    {
        for(auto sensor : sensorArray)
        {
            sensor->read(adc);
        }
    }    
}


#endif