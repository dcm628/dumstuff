#include "SensorClass.h"
#include <Arduino.h>
#include <ADC.h>
#include <array>
//#include <ADC_util.h>

using std::string;

// Constructor
//SENSOR::SENSOR(uint32_t setSensorID, uint32_t setSensorNodeID, string setSens_Name, SensorType setSensType, ADCType setADCType, uint32_t setADCinput, uint32_t setSens_Sample_rate, uint8_t setBitDepth)
//                : sensorID{setSensorID}, sensorNodeID{setSensorNodeID}, sens_name{setSens_Name}, senstype{setSensType}, adcType{setADCType}, ADCinput{setADCinput}, sens_sample_rate{setSens_Sample_rate}, bitDepth{setBitDepth}

SENSOR::SENSOR(uint32_t setSensorID, uint32_t setSensorNodeID, SensorType setSensType, ADCType setadcType,
               uint32_t setADCinput, uint32_t setSens_Sample_rate, uint8_t setBitDepth, int i)
                : sensorID{setSensorID}, sensorNodeID{setSensorNodeID}, senstype{setSensType}, adcType{setadcType}, ADCinput{setADCinput}, sens_sample_rate{setSens_Sample_rate}, bitDepth{setBitDepth}


{
    // place any constructor operations here
}

void SENSOR::begin()
{
    pinMode(ADCinput, INPUT);
}

void SENSOR::resetTimer()
{
    timer = 0;
}

void SENSOR::read(ADC* adc)
{
    //Add in sample rate code here to check if a sensor is up to be read
    //This is also where alternate ADC sources would be used - I do have the RTD sensors over ITC right now
    //I'll have to change how it's written though, right now it's ADC* adc which is specific to Teensy MCU ADC
    
    currentRawValue = adc->analogRead(ADCinput);
}

