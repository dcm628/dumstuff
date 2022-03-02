#ifndef STATELIST_H
#define STATELIST_H

// This header defines all the tracked states the system an be in, using an enum class for protection

enum class State
{
    // These are the God States, they can be reached from any position
    setup,      //
    debug,      // the outputs disabled for testing state?
    passive,    //
    test,       //
    abort,      //
    vent,       //
    offNominal, // off nominal is for when individual valves are actuated out of sequence


    // These states can only be accessed in sequence, from passive
    HiPressArm,
    HiPressPressurized,
    TankPressArm,
    TankPressPressurized,
    fireArmed,
    fire,

};


#endif