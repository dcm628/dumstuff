#include <Arduino.h>
#include <array>
#include <string>
#include <FlexCAN.h>
#include <ADC.h>
//#include <ADC_util.h>
#include <Wire.h>
#include <SD.h>
#include "ValveClass.h"
#include "ValveDefinitions.h"
#include "CANRead.h"
#include "CANWrite.h"
#include "ControlCommands.h"
#include "OperationFunctionTemplates.h"
#include "SensorClass.h"
#include "SensorDefinitions.h"
#include "StateList.h"

// Can Set Up
int busSpeed = 500000; //baudrate


uint32_t loopCount {0};// for debugging

// Set the global command, and global state
Command currentCommand{command_NOCOMMAND};
State currentState{State::setup};

// 2 timers to not print too much
elapsedMillis sensorTimer;
elapsedMillis timer2;

// set up built in SD card
SDClass builtInSD;

// ADC Setup
// Make the ADC OBJECT here
ADC* adc = new ADC();
#define PINS 23 //was default 25 but skipping two pins for I2C bus use?
#define PINS_DIFF 2
uint8_t adc_pins[] = {A0, A1, A2, A3, A6, A7, A8, A9, A10,
                      A11, A12, A13, A14, A15, A16, A17, A18, A19, A20, A21, A22, A23, A24};
uint8_t adc_pins_diff[] = {A10, A11};

bool input_enable[25] = {false, false, false, false, false, false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};



// -------------------------------------------------------------

void setup() 
{
  // Start Serial
  Serial.begin(115200);
  delay(5000);

  // Run Valve Setup
  valveSetUp(valveArray);

  // Run Sensor Setup
  for (int i = 0; i < PINS; i++)
    {
        pinMode(adc_pins[i], INPUT);
    }

  sensorSetUp(sensorArray);


  // Start Can
  Can0.begin(busSpeed);

  // SD set up
  pinMode(BUILTIN_SDCARD, OUTPUT);
  if(!builtInSD.begin(BUILTIN_SDCARD))
  {
    Serial.println("Built in SD card failed or not present.");
  }

  // set up the csv
  File dataFile = builtInSD.open("datalog.csv", FILE_WRITE);
  if(dataFile)
  {
    // sensor columns
    for (SENSOR* sensor : sensorArray)
    {
      std::string name{sensor->getSensorName()};
      dataFile.print(name.c_str());
      dataFile.print(", ");
    }

    // valve status column
    dataFile.println("ValveStatus, ");
  }
  dataFile.close();


  // ADC Set Up
  // reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 or ADC_REFERENCE::REF_EXT.
  //adc->setReference(ADC_REFERENCE::REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

  adc->adc0->setAveraging(4);                                             // set number of averages
  adc->adc0->setResolution(16);                                           // set bits of resolution
  adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS); // change the conversion speed
  adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED);       // change the sampling speed
  //adc->adc0->recalibrate();

///// ADC1 /////
  adc->adc1->setAveraging(4);                                             // set number of averages
  adc->adc1->setResolution(16);                                           // set bits of resolution
  adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS); // change the conversion speed
  adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED);        // change the sampling speed
  //adc->adc1->recalibrate();

  delay(500);



  
  Serial.println("Test Begin");

  sensorTimer = 0;
  timer2 = 0;
 
}

void loop() 
{
  // --- Step 1, Read CAN bus and update current command ---
  if(CANread(Can0, currentCommand))
  {
    Serial.print("Command Recieved: ");
    Serial.println(currentCommand);
  }

  // Process Commands Here


  // Advance needed valve tasks
  valveTasks(valveArray);


  // Peform sensor reads and write to CAN
  if(sensorTimer >= 2) // performs these operations only 500 times/second
  {
      // Read Sensors
      readSensors(sensorArray, adc);

      File dataFile = builtInSD.open("datalog.csv", FILE_WRITE);
      // Package and send sensor data, write to SD card
      for(SENSOR* sensor : sensorArray)
      {
        int reading = sensor->getCurrentRawValue();
        std::array<uint8_t, 2> messageArray{};

        messageArray.at(0) = (reading >> 8) & 0xff;
        messageArray.at(1) = reading & 0xff;

        CANwrite(Can0, messageArray, 0); // need IDs for different returns

        // write to datafile
        if(dataFile)
        {
          dataFile.print(reading);
          dataFile.print(", ");
        }

      }

      // Get and send valve status
      std::bitset<BITFLAG_SIZE> valveFlags{setValveFlags(valveArray)};
      std::array<uint8_t, 2> flagArray{};
      flagArray.at(0) = (valveFlags >> 8).to_ulong() & 0xff;
      flagArray.at(1) = valveFlags.to_ulong() & 0xff;

      CANwrite(Can0, flagArray, 0);   // need IDs for different returns

      // write valve status to SD
      if(dataFile)
      {
        dataFile.print(valveFlags.to_ulong());
        dataFile.println(", ");
      }
      dataFile.close();
  }



  

  

  



}