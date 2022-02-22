#ifndef SENSORCLASS_H
#define SENSORCLASS_H

// THIS FILE WAS FORMERLY SensorHeader.h

#include <Arduino.h>
#include <string>
#include <bitset>
#include <ADC.h>


//using std::string;


// enum for holding sensor types
enum SensorType
{
  pt,
  loadcell1000,
  tcktype,
  rtd,
};

// enum for holding ADC input types
enum ADCType
{
  TeensyMCUADC,
  ADS1258,  //not in use yet
  ADS1263,  //not in use yet
  I2Cext, //currently used for RTD
};

class SENSOR
{
  private:
    const uint32_t sensorID;
    const uint32_t sensorNodeID;                      // NodeID the valve is controlled by
    //const string sens_name;           //your own name for sensor to reference it
    const SensorType senstype;        //sensor type from pt,tc,load_cell, or any others added (update options here as they are added for reference) (Changed this to enum)
    const ADCType adcType;
    const uint32_t ADCinput;               //the input that will be read for this sensor that will get used in the ADC read main loop
    const uint32_t sens_sample_rate;       //the sample rate this given sensor will be read at
    elapsedMillis timer;              // timer for future sensor timing operations
    uint32_t currentRawValue{};            // holds the current value for the sensor
    const uint8_t bitDepth;               // bit depth of the sample

  public:
    //SENSOR(uint32_t setSensorID, uint32_t setSensorNodeID, string setSens_Name, SensorType setSensType, ADCType setadcType, uint32_t setADCinput, uint32_t setSens_Sample_rate, uint8_t setBitDepth);
    SENSOR(uint32_t setSensorID, uint32_t setSensorNodeID, SensorType setSensType, ADCType setadcType, uint32_t setADCinput, uint32_t setSens_Sample_rate, uint8_t setBitDepth);

    // Access functions defined in place
    uint32_t getSensorID(){return sensorID;}
    uint32_t getSensorNodeID(){return sensorNodeID;}
    //string getSensorName(){return sens_name;}
    SensorType getSensorType(){return senstype;}
    ADCType getADCType(){return adcType;}
    uint32_t getADCinput(){return ADCinput;}
    uint32_t getSensSampleRate(){return sens_sample_rate;}
    uint32_t getCurrentRawValue(){return currentRawValue;}
    uint8_t getBitDepth(){return bitDepth;}

    // further fuctions defined in SensorClass.cpp
    void begin();                     // run in setup to get pins going
    
    void resetTimer();                // resets timer to zero

    void read(ADC* adc);              // updates currentRawValue with current reading, using an activated ADC object

};

// need to add differential read toggle somehow 
// - differential boolean variable that allows second input to be chosen or defaulted to correct option
// need to add a way to set other SENSOR types like the RTD sensors over I2C (we'd probably want multiple classes. ADCsensors, I2C sensors, SPI sensors etc - Mat)
// - maybe not the right call to roll into this? Hmm. Need to establish use of SENSOR class with sample rates and real read/sends to see what is better
// That will set me up for incorporating the external ADCs later



#endif