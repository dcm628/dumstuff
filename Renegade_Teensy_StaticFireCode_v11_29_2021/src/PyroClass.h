#ifndef PyroClass_H
#define PyroClass_H

#include "PyroStates.h"
#include <Arduino.h>


enum PyroType
{
    Solo,
    Cloned,
};



class Pyro
{
    private:
        const int pyroID;
        const int pyroNodeID;
        const int firePin;
        //const int shuntPin;
        //const int contCheckPin;
        const PyroType pyroType;
        //const int clonedPyroID;
        const uint32_t fireDelay;
        elapsedMicros timer;
        PyroState state;
        const uint32_t liveOutTime;

    public:
    
    // constructor, define the valve ID here, and the pin that controls the valve, setFireDelay is only parameter that can be left blank
        Pyro(int setPyroID, int setPyroNodeID, PyroType setPyroType, int setFirePin, uint32_t setFireDelay, uint32_t setLiveOutTime); 
    // Alternate constructor with future full implementation, needs the clonedpyro features still
    //    Pyro(int setPyroID, int setPyroNodeID, int setFirePin, int setShuntPin, int setContPin, uint32_t setFireDelay = 0);

    // a start up method, to set pins from within setup()
        void begin();

    // access functions defined in place

    // get functions, return the current value of that variable
        uint32_t getPyroID(){return pyroID;}
        uint32_t getPyroNodeID(){return pyroNodeID;}
        PyroType getPyroType(){return pyroType;}
        uint32_t getFirePin(){return firePin;}
        //uint32_t getshuntPin(){return shuntPin;}
        //uint32_t getContPin(){return contCheckPin;}        
        uint32_t getLiveOutTime(){return liveOutTime;}
        uint32_t getFireDelay(){return fireDelay;}
        PyroState getState(){return state;}
        uint32_t getTimer(){return timer;}

    // set functions, allows the setting of a variable
        void setState(PyroState newState) {state = newState; timer = 0;} //every time a state is set, the timer should reset

    // functions with executables defined in ValveClasses.cpp
        void resetTimer();              // resets timer to zero, timer increments automatically in microseconds

    // ----- THIS METHOD TO BE RUN EVERY LOOP ------
    // stateOperations will check the current state of the valve and perform any actions that need to be performed
    // for example, if the valve is commanded to open, this needs to be run so that the valve can start opening
    // and it needs to be run every loop so that once enough time has pass the 
        void stateOperations();



};

#endif