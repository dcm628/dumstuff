#ifndef CONTROLCOMMANDS_H
#define CONTROLCOMMANDS_H

// this enum hold all commands to be sent to the teensy. All commands assumed to be uint8_t type 

// in main file as global declare: Command CurrentCommand{command_NOCOMMAND} to hold the current command

enum Command
{
    // we reserve 0 to be a no command state
    command_NOCOMMAND = 0,
    command_debug = 1,                      //Start of global states
    command_passive = 3,
    command_test = 5,
    command_abort = 7,
    command_vent = 9,
    command_HiPressArm = 11,
    command_HiPressPressurized = 13,
    command_TankPressArm = 15,
    commend_TankPressPressurized = 17,
    command_fireArm = 19,
    command_fire = 21,                      //End of global states
    command_closeHiPress = 32,              //Start of individual device states
    command_openHiPress = 33,
    command_closeHiPressVent = 34,
    command_openHiPressVent = 35,
    command_closeLoxVent = 36,
    command_openLoxVent = 37,
    command_closeLoxDomeReg = 38,
    command_openLoxDomeReg = 39,
    command_closeLoxDomeRegVent = 40,
    command_openLoxDomeRegVent = 41,
    command_closeFuelVent = 42,
    command_openFuelVent = 43,
    command_closeFuelDomeReg = 44,
    command_openFuelDomeReg = 45,
    command_closeFuelDomeRegVent = 46,
    command_openFuelDomeRegVent = 47,
    command_closeFuelMV = 48,
    command_openFuelMV = 49,
    command_closeLoxMV = 50,
    command_openLoxMV = 51,                 //End of individual device states
    command_disableHiPressHiVentSafety = 52,            //Start of safety enables
    command_enableHiPressHiVentSafety = 53,
    command_disableFuelVentSafety = 54,
    command_enableFuelVentSafety = 55,
    command_disableLoxDomeRegLoxDomeVentSafety = 56,
    command_enableLoxDomeRegLoxDomeVentSafety = 57,    
    command_disableFuelDomeRegFuelDomeVentSafety = 58,
    command_enableFuelDomeRegFuelDomeVentSafety = 59,    
    command_disableLoxVentSafety = 60,
    command_enableLoxVentSafety = 61,
    command_disableMainValvesSafety = 62,
    command_enableMainValvesSafety = 63,                //End of safety enables
    command_SIZE, // not a valid command but it is useful for checking if recieved messages are valid commands, see CANRead. Always leave this at the end of the enum listcomm
};




#endif