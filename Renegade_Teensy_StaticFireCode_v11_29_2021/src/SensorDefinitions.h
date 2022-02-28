#ifndef SENSORDEFINITIONS_H
#define SENSORDEFINITIONS_H

#include "SensorClass.h"
#include <array>
#include <ADC.h>


///////////////////ALLLL OF THIS IS WRONG AND THE OLD PASAFIRE SENSORS 

// define number of sensors here
#define NUM_SENSORS 11

// initialize all sensor objects here (PINS NOT CORRECT)
SENSOR pt0{0, 2, ChamberPT1, TeensyMCUADC, pt,  A14, 500, 16};
SENSOR pt1{1, 3, FuelTankPT, TeensyMCUADC, pt,  A15, 500, 16};
SENSOR pt2{2, 3, LOXTankPT,TeensyMCUADC, pt,  A16, 500, 16};
SENSOR pt3{3, 3, FuelRegPT, TeensyMCUADC,pt,  A17, 500, 16};
SENSOR pt4{4, 3, LOXRegPT, TeensyMCUADC,pt,  A18, 500, 16};
SENSOR pt5{5, 2, FuelLinePT, TeensyMCUADC,pt,  A19, 500, 16};
SENSOR pt6{6, 2, LOXLinePT, TeensyMCUADC,pt,  A20, 500, 16};
SENSOR pt7{7, 3, PneumaticPT, TeensyMCUADC,pt,  A21, 500, 16};
SENSOR load_cell1{8, 2, ThrustMountLoadCell1, TeensyMCUADC,loadcell1000,  A22, 100, 16};


// place sensor objects into this array
std::array<SENSOR*, NUM_SENSORS> sensorArray{&pt1, &pt2, &pt3, &pt4, &pt5, &pt6, &pt7, &load_cell1};




#endif