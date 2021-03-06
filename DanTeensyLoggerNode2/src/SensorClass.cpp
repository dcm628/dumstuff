#include "SensorClass.h"
#include <Arduino.h>
#include <ADC.h>
#include <array>
#include <InternalTemperature.h>
//#include <ADC_util.h>

using std::string;

// Constructor
//SENSOR::SENSOR(uint32_t setSensorID, uint32_t setSensorNodeID, string setSens_Name, SensorType setSensType, ADCType setADCType, uint32_t setADCinput, uint32_t setSens_Sample_rate, uint8_t setBitDepth)
//                : sensorID{setSensorID}, sensorNodeID{setSensorNodeID}, sens_name{setSens_Name}, senstype{setSensType}, adcType{setADCType}, ADCinput{setADCinput}, sens_sample_rate{setSens_Sample_rate}, bitDepth{setBitDepth}

MCU_SENSOR::MCU_SENSOR(uint32_t setSensorID, uint32_t setSensorNodeID, uint8_t setADCinput, uint32_t setSampleRateSlowMode, uint32_t setSampleRateMedMode, uint32_t setSampleRateFastMode, bool setInternalMCUTemp, float setLinConvCoef1_m, float setLinConvCoef1_b, float setLinConvCoef2_m = 1, float setLinConvCoef2_b = 0, uint32_t setCurrentSampleRate, SensorState setSensorState, bool setNodeIDCheck, bool setNewSensorValueCheck, bool setNewConversionCheck)
                : sensorID{setSensorID}, sensorNodeID{setSensorNodeID}, ADCinput{setADCinput}, sampleRateSlowMode{setSampleRateSlowMode}, sampleRateMedMode{setSampleRateMedMode}, sampleRateFastMode{setSampleRateFastMode}, internalMCUTemp{setInternalMCUTemp}, linConvCoef1_m{setLinConvCoef1_m}, linConvCoef1_b{setLinConvCoef1_b}, linConvCoef2_m{setLinConvCoef2_m}, linConvCoef2_b{setLinConvCoef2_b}, currentSampleRate{setCurrentSampleRate}, sensorState{setSensorState}, nodeIDCheck{setNodeIDCheck}, newSensorValueCheck{setNewSensorValueCheck}, newConversionCheck{setNewConversionCheck}


{
    // place any constructor operations here
}

void MCU_SENSOR::begin()
{
    if (nodeIDCheck)
    {
        pinMode(ADCinput, INPUT);
    }
}

void MCU_SENSOR::resetTimer()
{
    timer = 0;
}

void MCU_SENSOR::read(ADC* adc)
{
    //Add in sample rate code here to check if a sensor is up to be read
    //This is also where alternate ADC sources would be used - I do have the RTD sensors over ITC right now
    //I'll have to change how it's written though, right now it's ADC* adc which is specific to Teensy MCU ADC
    if (currentSampleRate != 0)     //math says no divide by zero, use separate conditional for sample rate of 0
    {
    if (timer >= (1000000/currentSampleRate))   // Divides 1 second in microseconds by current sample rate in Hz
        {
            if (internalMCUTemp)
            {
                currentRawValue = InternalTemperature.readTemperatureC();
            }
            else
            {
                currentRawValue = adc->analogRead(ADCinput);
                /////linear conversions here, y = m*x + b
                // This automatically stores converted value for the on board nodes
                //currentConvertedValue = linConvCoef1_m*currentRawValue + linConvCoef1_b;
            }
/*             Serial.print("sensorID: ");
            Serial.print(sensorID);
            Serial.print(", currentRawValue: ");
            Serial.println(currentRawValue);
            Serial.print(", currentConvertedValue: ");
            Serial.println(currentConvertedValue); */
            
            newSensorValueCheck = true;
            newConversionCheck = false;
            timer = 0;
        }
    }
}

void MCU_SENSOR::stateOperations()
{

    switch (sensorState)
    {
    case SensorState::Off:
        setCurrentSampleRate(0);
        break;
    case SensorState::Slow:
        setCurrentSampleRate(sampleRateSlowMode);
        break;
    case SensorState::Medium:
        setCurrentSampleRate(sampleRateMedMode);
        break;
    case SensorState::Fast:
        setCurrentSampleRate(sampleRateFastMode);
        break;
    // All other states require no action
    default:
        break;
    }
}

void MCU_SENSOR::linearConversion()
{
    /////linear conversions here, y = m*x + b
    if (newSensorValueCheck && newConversionCheck == false)
    {
    currentConvertedValue = linConvCoef1_m*currentRawValue + linConvCoef1_b;    //Initial Calibration
    currentConvertedValue = linConvCoef2_m*currentConvertedValue + linConvCoef2_b;    //Secondary Calibration
    newConversionCheck = true;

            Serial.print("sensorID: ");
            Serial.print(sensorID);
            Serial.print(", currentRawValue: ");
            Serial.println(currentRawValue);
            Serial.print(", currentConvertedValue: ");
            Serial.println(currentConvertedValue);
    }
}