#ifndef VALVEDEFINITIONS_H
#define VALVEDEFINITIONS_H

#include "ValveClass.h"
#include <array>

// Define number of valves here
#define NUM_VALVES 10
#define NUM_VALVEENABLE 6

// Declare all Valve Objects here using ValveClass, and add them to the valveArray
// On Engine Node
Valve HiPress{16, 2, NormalClosed, 2, 25000, false};
Valve HiPressVent{17, 2, NormalClosed, 5, 25000, false};
Valve LoxMV{27, 2, NormalClosed, 7, 25000, 12000000, true};
Valve FuelMV{26, 2, NormalClosed, 6, 25000, 12000000, true};
// On Prop Node
Valve LoxVent{18, 3, NormalOpen, 2, 500000, 9, 166, false};
Valve LoxDomeReg{19, 3, NormalClosed, 5, 25000, false};
Valve LoxDomeRegVent{20, 3, NormalClosed, 6, 25000, false};
Valve FuelVent{21, 3, NormalClosed, 2, 25000, false};
Valve FuelDomeReg{22, 3, NormalClosed, 7, 25000, false};
Valve FuelDomeRegVent{23, 3, NormalClosed, 8, 25000, false};

// ADD VALVES TO THIS VALVE ARRAY IN THE FORM: &VALVE

std::array<Valve*, NUM_VALVES> valveArray{&HiPress, &HiPressVent, &LoxVent, &LoxDomeReg, &LoxDomeRegVent, &FuelVent, &FuelDomeReg, &FuelDomeRegVent, &FuelMV, &LoxMV};

// Declare all ValveEnable Objects here
// On Engine Node
ValveEnable HiPressHiVentSafe{24, 2};
ValveEnable MainValvesSafe{25, 2};
// On Prop Node
ValveEnable FuelVentSafe{24, 3};
ValveEnable LoxDomeRegVentSafe{25, 3};
ValveEnable FuelDomeRegVentSafe{26, 3};
ValveEnable LoxVentSafe{27, 3};

std::array<ValveEnable*, NUM_VALVEENABLE> valveEnableArray{&HiPressHiVentSafe, &MainValvesSafe, &FuelVentSafe, &LoxDomeRegVentSafe, &FuelDomeRegVentSafe, &LoxVentSafe};

#endif