#ifndef VALVEDEFINITIONS_H
#define VALVEDEFINITIONS_H

#include "ValveClass.h"
#include <array>

// Define number of valves here
#define NUM_VALVES 10
#define NUM_VALVEENABLE 6
/* 
//Pasafire Define Numbers Only Here - Uncomment these and comment out BLT ones
#define NUM_VALVES 6
#define NUM_VALVEENABLE 4
 */

// Declare all Valve Objects here using ValveClass, and add them to the valveArray
// On Engine Node
Valve HiPress{16, 2, NormalClosed, 2, 25000, false};
Valve HiPressVent{17, 2, NormalClosed, 5, 25000, false};
Valve LoxMV{24, 2, NormalClosed, 7, 25000, false, -1, 50};
Valve FuelMV{25, 2, NormalClosed, 6, 25000, false, -1, 50};
// On Prop Node
Valve LoxVent{18, 3, NormalOpen, 2, 500000, false, 166};
Valve LoxDomeReg{19, 3, NormalClosed, 5, 25000, false};
Valve LoxDomeRegVent{20, 3, NormalClosed, 6, 25000, false};
Valve FuelVent{21, 3, NormalClosed, 2, 25000, false};
Valve FuelDomeReg{22, 3, NormalClosed, 7, 25000, false};
Valve FuelDomeRegVent{23, 3, NormalClosed, 8, 25000, false};
// On Pasafire Node
/* Valve LoxVent{18, 8, NormalOpen, 2, 500000, false, 166};
Valve LoxDomeReg{19, 8, NormalOpen, 5, 25000, false};
Valve FuelVent{21, 8, NormalClosed, 2, 25000, false};
Valve FuelDomeReg{22, 8, NormalClosed, 7, 25000, false};
Valve LoxMV{24, 8, NormalClosed, 7, 25000, false, -1, 50};
Valve FuelMV{25, 8, NormalClosed, 6, 25000, false, -1, 50};
 */

// ADD VALVES TO THIS VALVE ARRAY IN THE FORM: &VALVE

std::array<Valve*, NUM_VALVES> valveArray{&HiPress, &HiPressVent, &LoxVent, &LoxDomeReg, &LoxDomeRegVent, &FuelVent, &FuelDomeReg, &FuelDomeRegVent, &FuelMV, &LoxMV};
//Pasafire Valve Array
//std::array<Valve*, NUM_VALVES> valveArray{&LoxVent, &LoxDomeReg, &FuelVent, &FuelDomeReg, &FuelMV, &LoxMV};


// Declare all ValveEnable Objects here
// THESE ARE IN THE WRONG ORDER AND WE KNOW THAT, WEIRD ISSUES WITH INSTANTIATING VALVE ENABLES INCORRECTLY
// LISTED AS {ValveEnableID, Pin, ValveEnableNodeID}

// On Engine Node
ValveEnable HiPressHiVentSafe{32, 24, 2};
ValveEnable MainValvesSafe{37, 25, 2};
// On Prop Node
ValveEnable FuelVentSafe{33, 24, 3};
ValveEnable LoxDomeRegVentSafe{34, 25, 3};
ValveEnable FuelDomeRegVentSafe{35, 26, 3};
ValveEnable LoxVentSafe{36, 27, 3};
// On Pasafire Node
/* 
ValveEnable FuelVentSafe{33, 24, 8};
ValveEnable LoxVentSafe{36, 27, 8};
ValveEnable MainValvesSafe{37, 25, 2};
ValveEnable LoxFuelDomesSafe{38, 25, 2}; 
*/


std::array<ValveEnable*, NUM_VALVEENABLE> valveEnableArray{&HiPressHiVentSafe, &MainValvesSafe, &FuelVentSafe, &LoxDomeRegVentSafe, &FuelDomeRegVentSafe, &LoxVentSafe};
// Pasafire Valve Enable Array
//std::array<ValveEnable*, NUM_VALVEENABLE> valveEnableArray{&FuelVentSafe, &LoxVentSafe, &MainValvesSafe, &LoxFuelDomesSafe};


#endif