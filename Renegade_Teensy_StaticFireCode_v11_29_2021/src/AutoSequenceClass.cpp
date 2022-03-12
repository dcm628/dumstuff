#include "AutoSequenceClass.h"
//#include "AutoSequenceStates.h"
#include <Arduino.h>


AutoSequence::AutoSequence(uint32_t setCountdownStart, uint32_t setHostNodeID) : countdownStart{setCountdownStart}, hostNodeID{setHostNodeID}
{

}

void AutoSequence::begin()
{
    //AutoSequenceState::Standby;
}



void AutoSequence::stateOperations()
{
    switch (state)
    {
    case AutoSequenceState::Standby:
        
        break;

    
    case AutoSequenceState::RunCommanded:
        
        timer = 0;
        state = AutoSequenceState::Running;
        

        break;
        
       
    case AutoSequenceState::Running:
        //if (hostNodeID = nodeID)
        //{
            countdownStart = getCountdownStart();
            timer = getTimer();
            currentCountdown = timer - countdownStart;  //countdownstart is stored as unsigned and treated as negative here in the math
            setCurrentCountdown(currentCountdown);
        //}
        break;

       
    case AutoSequenceState::Hold:
        

        
        break;
    
    // All other states require no action
    default:
        break;
    }
}



/* void AutoSequence::resetTimer()
{
    timer = 0;
} */