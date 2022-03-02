#include "PyroClass.h"
#include "PyroStates.h"
#include <Arduino.h>



Pyro::Pyro(uint32_t setPyroID, uint32_t setPyroNodeID, PyroType setPyroType, int setFirePin, uint32_t setFireDelay, uint32_t setLiveOutTime)
                : pyroID{setPyroID}, pyroNodeID{setPyroNodeID}, pyroType{setPyroType}, firePin{setFirePin}, fireDelay{setFireDelay}, liveOutTime{setLiveOutTime}
{
    switch (pyroType)
    {
        case Solo:
            state = PyroState::Off;
            break;
        case Cloned:
            state = PyroState::Off;
            break;
        default:
            state = PyroState::Off;
            break;
    }
    timer = 0;

}

void Pyro::begin()
{
    pinMode(firePin, OUTPUT);
    digitalWrite(firePin, 0);
}

void Pyro::resetTimer()
{
    timer = 0;
}


void Pyro::stateOperations()
{
    switch (state)
    {
    // if a valve has been commanded to fire, it will start actuation after appropriate delay, normal closed actuate open, normal open actuate closed
    // every state change should reset the timer
    case PyroState::FireCommanded:
        if(timer >= fireDelay)
        {
            state = PyroState::OnCommanded;
            timer = 0;
            break;
  
        }
        break;

    // if a pyro is commanded on, turns on 
    case PyroState::OnCommanded:
        state = PyroState::On;
        timer = 0;
        break;

    case PyroState::On:
        digitalWrite(firePin, 1);
        if(timer >= liveOutTime)
        {
            state = PyroState::Off;
            timer = 0;
        }
        break;

    // if a pyro is commanded off, turns off immediately, not sure I need this at all the way we do on valves
    case PyroState::OffCommanded:
        state = PyroState::Off;
        timer = 0;
        break;
        
    case PyroState::Off:
        digitalWrite(firePin, 0);
        timer = 0;
        break;        
    
    // All other states require no action
    default:
        break;
    }
}