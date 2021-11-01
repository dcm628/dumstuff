// Renegade CAN Data logger and SF Stand message interpreter
// Unfortunately by Dan


#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <SD.h>
#include <SPI.h>
#include <SensorDefinitions.h>
#include <SensorClass.h>


FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> PropCAN;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> GUICAN;
CAN_message_t msg;

elapsedMillis sinceGUIsend;




/*
  SD card read/write
 
 This example shows how to read and write data to and from an SD card file 	
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11, pin 7 on Teensy with audio board
 ** MISO - pin 12
 ** CLK - pin 13, pin 14 on Teensy with audio board
 ** CS - pin 4, pin 10 on Teensy with audio board
 
 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 
 This example code is in the public domain.
 	 
 */
 


File myFile;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// Teensy audio board: pin 10
// Teensy 3.5 & 3.6 & 4.1 on-board: BUILTIN_SDCARD
// Wiz820+SD board: pin 4
// Teensy 2.0: pin 0
// Teensy++ 2.0: pin 20
const int chipSelect = 10;

void setup()
{

PropCAN.begin();
GUICAN.begin();

PropCAN.setBaudRate(500000);
GUICAN.setBaudRate(500000);



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


  
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect.
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  // open the file. 
  myFile = SD.open("test.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
	// close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  
  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");
    
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
    	Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
  	// if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

void loop()
{
/// CAN read on CAN bus from prop stand/rocket
  PropCAN.read(msg);
  if msg.id = SENSOR 


/// CAN send on CAN bus to GUI, every 100 ms
  if (sinceGUIsend >= 100) {
    GUICAN.write(msg)


  }
  sinceGUIsend = 0;
  

}

