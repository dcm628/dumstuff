#ifndef SENSORDEFINITIONS_H
#define SENSORDEFINITIONS_H

#include "SensorClass.h"
#include <array>
#include <ADC.h>


///////////////////ALLLL OF THIS IS WRONG AND THE OLD PASAFIRE SENSORS 

// define number of sensors here
#define NUM_SENSORS 11

// initialize all sensor objects here (PINS NOT CORRECT)
SENSOR pt0 {0, 2, "ChamberPT1", pt, TeensyMCUADC, A14, 500, 16};
SENSOR pt1 {1, 3, "FuelTankPT", pt, TeensyMCUADC, A15, 500, 16};
SENSOR pt2 {2, 3, "LOXTankPT", pt, TeensyMCUADC, A16, 500, 16};
SENSOR pt3 {3, 3, "FuelRegPT", pt, TeensyMCUADC, A17, 500, 16};
SENSOR pt4 {4, 3, "LOXRegPT", pt, TeensyMCUADC, A18, 500, 16};
SENSOR pt5 {5, 2, "FuelLinePT", pt, TeensyMCUADC, A19, 500, 16};
SENSOR pt6 {6, 2, "LOXLinePT", pt, TeensyMCUADC, A20, 500, 16};
SENSOR pt7 {7, 3, "PneumaticPT", pt, TeensyMCUADC, A21, 500, 16};
SENSOR load_cell1 {8,  2,"ThrustMountLoadCell1", loadcell1000, TeensyMCUADC, A22, 100, 16};


// place sensor objects into this array
std::array<SENSOR*, NUM_SENSORS> sensorArray{&pt1, &pt2, &pt3, &pt4, &pt5, &pt6, &pt7, &load_cell1};




#endif