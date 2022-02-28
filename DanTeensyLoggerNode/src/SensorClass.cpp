#include "SensorClass.h"
#include <Arduino.h>
#include <ADC.h>
#include <array>
//#include <ADC_util.h>

using std::string;

SENSOR::SENSOR(string setSens_Name, SensorType setSensType, bool setDiffRead, int setADCinput, int setSens_Sample_rate_high, int setSens_Sample_rate_low, int setNodeID, int setCANID)
                : sens_name{setSens_Name}, senstype{setSensType}, DiffRead{setDiffRead}, ADCinput{setADCinput}, sens_sample_rate_high{setSens_Sample_rate_high}, sens_sample_rate_low{setSens_Sample_rate_low}, nodeid{setNodeID}, CANID{setCANID}
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
    currentRawValue = adc->analogRead(ADCinput);
}