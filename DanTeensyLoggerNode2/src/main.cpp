// Renegade CAN Data logger and SF Stand message interpreter
// Unfortunately by Dan

#include <Arduino.h>
#include <FlexCAN.h>
#include <SD.h>
#include <EEPROM.h>
#include <array>
#include <string>
#include <list>
using std::string;
#include <ADC.h>
#include "Adafruit_MCP9808.h"
#include "ToMillisTimeTracker.h"
#include "CANReports.h"
//#include "CANReportsGUI.h"
#include "CANRead.h"
#include "CANWrite.h"
#include "SensorDefinitions.h"
#include "PyroDefinitions.h"
#include "ValveDefinitions.h"
#include "AutoSequenceDefinitions.h"



#include "C:\Users\Daniel Morgan\Documents\PlatformIO\Projects\RocketDriver\src\OperationFunctionTemplates.h"




//Trying to figure out RTC stuff with these libs
#include <TimeLib.h>
#include <DS1307RTC.h>

#define STATE_INFO_ID_RANGE 50      //Max array elements for CANID passthrough array
#define LOGGER_NODE_ID 6

///// ADC ///// - Not intended for use on logger, but here to keep sensor tasks function common
ADC* adc = new ADC();

uint8_t nodeID = LOGGER_NODE_ID;     //Fixed nodeID for Logger-Interpreter node since it has unique code

SDClass teensy_sd;
File output_file;

void logDataStartup() {
    // Initialize the SD.
    teensy_sd.begin(BUILTIN_SDCARD);
    String output_name = "Output0.csv";
    bool file_created = false;
    for(int i = 1; i < 4096 && !file_created; i++) {
        if(teensy_sd.exists(output_name.c_str())) {
            output_name = "Output" + String(i) + ".csv";
        }
        else {
            file_created = true;
        }
    }
    output_file = teensy_sd.open("output.csv", FILE_WRITE);
    output_file.print("Message timestamp,Message ID,Message");
    output_file.flush();
}



CAN_message_t message0;
CAN_message_t message1;
elapsedMillis sinceGUIsend;

///// ALL CAN IDs for messages that are to be passed through to the GUI PI, other than direct sensor reports
//or I change it to allocate a certain range of possible Report IDs
uint8_t CANbytesPassthroughArray[STATE_INFO_ID_RANGE][8];      //position = CANID, rows 0-7 are CAN bytes
uint16_t CANTimestampPassthroughArray[STATE_INFO_ID_RANGE];      //position = CANID, value is FlexCAN timestamp
uint8_t CANIDARRAYLen[STATE_INFO_ID_RANGE];

///// TODO - Setup the pass throughs to all retransmit with extended ID fields, make extended IDs timestamps.



//The Pressure calculated
int Pressure;
//stupid bit math
float byte0, byte1, bitshift, value;

//This looks awful, but it creates arrays to store every possible CANID frame's bytes to then retransmit

//float CANIDARRAYBytes[2048][8]; //array with positions for all possible CANIDs in 11 bit standard ID format for storing recent messages
//uint8_t CANIDARRAYLen[50];

//array for using in the values converted from GUI
//This exists so that any value that doesn't get a conversion applied still gets sent to the Pi.
//That automates sending the state report frames, anything we didn't convert is passed as is.
//It also sends any frames that happen to not have been in our conversion code incidentally to the Pi for it's low rate logging code to still catch.
//This could be structured as ints instead of 2D array, but then you need to write CAN code to chop up the ints automatically for the sends.
//int CANIDARRAYConvertedBytes[2048][8]; 

int counter = 0;


//float PtConversion[2048][2];





void setup()
{
    delay(250);
    Serial.println("Initializing SD.");
    Can0.begin(500000);
    Can1.begin(500000);
    
  // -----Run Sensor NodeID Check-----
  SensorNodeIDCheck(sensorArray, nodeID);    
    
  // -----Run Sensor Setup -----
  sensorSetUp(sensorArray);
  
  
      //[Sensor ID]
    //[][0] = M Value
    //[][1] = B Value
    // Dome Reg Fuel PT
    /* PtConversion[79][0] = 0.0186;
    PtConversion[79][1] = -126.67;
    // Dome Reg LOx PT
    PtConversion[80][0] = 0.0185;
    PtConversion[80][1] = -133.36;
    // Fuel Tank PT
    PtConversion[81][0] = 0.0186;
    PtConversion[81][1] = -129.3;
    // LOx Tank PT
    PtConversion[82][0] = 0.0187;
    PtConversion[82][1] = -125.36;
    // Fuel High Press PT
    PtConversion[83][0] = 0.0933;
    PtConversion[83][1] = -638.38;
    // LOx High Press PT
    PtConversion[84][0] = 0.093;
    PtConversion[84][1] = -629.72;
    // MV Pneumatic PT
    PtConversion[78][0] = 0.0186;
    PtConversion[78][1] = -126.56;
    // Chamber PT 1
    PtConversion[56][0] = 0.0185;
    PtConversion[56][1] = -128.88;
    // Chamber PT 2
    PtConversion[55][0] = 0.0186;
    PtConversion[55][1] = -102.94;
    // Fuel Injector PT
    PtConversion[58][0] = 0.0186;
    PtConversion[58][1] = -123.27;
    // Fuel Inlet Prop Side PT
    PtConversion[57][0] = 0.0185;
    PtConversion[57][1] = -125.74;
    // LOx Inlet Prop Side PT
    PtConversion[59][0] = 0.0186;
    PtConversion[59][1] = -128.58; */
    logDataStartup();
}

void loop()
{
myTimeTrackingFunction();

/// CAN read on CAN bus from prop stand/rocket
  if(Can0.read(message0)) 
    {
      // Print read CAN messages into the SD card ring buffer
      String hexMsg = "";
      for(unsigned char i : message0.buf) {
          hexMsg += String(i, HEX);
      }

      String out = String(message0.timestamp) + "," + message0.id + "," + hexMsg;
      output_file.println(out);
      output_file.flush();
      Serial.println(out);

      if (message0.id >= STATE_INFO_ID_RANGE)
      {
      
      // checks if incoming CAN message has ID matching a sensor, and stores information in sensor objects if it does
        for (auto sensor : sensorArray)
        {
            if (sensor->getSensorID() == message0.id)
            {
              sensor->setCurrentRawValue(message0.buf[0] + (message0.buf[1] << 8));
              sensor->setCANTimestamp(message0.timestamp);
              sensor->setNewSensorValueCheck(true);
              sensor->setNewConversionCheck(false);
            }
            
        }
      }
      // Manually setting state reports and autosequence reports to pass through
      else
      {
        CANIDARRAYLen[message0.id] = message0.len;
        CANTimestampPassthroughArray[message0.id] = message0.timestamp;
          for (size_t j = 0; j < message0.len; j++)
          {
            CANbytesPassthroughArray[message0.id][j] = message0.buf[j];
          }
      }
    }

/// CAN send on CAN bus to GUI, 10Hz rate
  if (sinceGUIsend >= 1000) {
    Serial.println("nothing to see here");

sensorTasks(sensorArray, adc, nodeID);
SensorArrayConvertedCANSend(Can1, sensorArray);

/*   
  for (int i = 0; i < 2048; i++)
  {
    if (CANIDARRAYBytes[i][0] != 0){
      if (PtConversion[i][0] != 0){
    byte0 = CANIDARRAYBytes[i][0];
    byte1 = CANIDARRAYBytes[i][1];
    bitshift = byte0*256;
    value = bitshift+byte1;
    Pressure = (int)(value * PtConversion[i][0] + PtConversion[i][1]);
    CANIDARRAYConvertedBytes[i][0] = (Pressure >> 8) & 0xff;
    CANIDARRAYConvertedBytes[i][1] = Pressure & 0xff;

    }
    Serial.print(value);
    Serial.print("  ");
    Serial.print(byte0);
    Serial.print("  ");
    Serial.print(byte1);
    Serial.print("  ");
    Serial.print(Pressure);
    Serial.print("  ");
    Serial.print(bitshift);
    Serial.println();
    }
  } */
    
/////BELOW IS THE CANSEND CODE, as long as you pack the converted values into CANIDARRAYConvertedBytes it will handle everything
  for (int j = 0; j < STATE_INFO_ID_RANGE; j++)
  {
    if (CANIDARRAYLen[j] != 0) {

      
      message1.ext = 1;
      message1.id = j + CANTimestampPassthroughArray[j]<<11;
      message1.len = CANIDARRAYLen[j];
      
      
      for (int k = 0; k < message1.len; k++)
      { 
        message1.buf[k] = CANbytesPassthroughArray[j][k];
        // Serial.print(CANbytesPassthroughArray[j][k]);
        // Serial.println();
      }
      
      // Serial.print("Length of message 1  after  ");
      // Serial.print(message1.len);
      //Serial.println(message1.id);
      //Serial.print(message1.buf[0]);
      //Serial.print(", ");
      //Serial.print(message1.buf[1]);
      Can1.write(message1);
    }
  }
  counter = counter+1;
  Serial.println();
  Serial.print("GUI Frame Loop Counter: ");
  Serial.print(counter);
  Serial.println();
  sinceGUIsend = 0;







}