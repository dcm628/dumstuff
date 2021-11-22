// Renegade CAN Data logger and SF Stand message interpreter
// Unfortunately by Dan


#include <Arduino.h>
#include <FlexCAN.h>
#include <kinetis_flexcan.h>
#include <WireKinetis.h>
#include <SD.h>
#include "SdFat.h"
#include "sdios.h"
#include "RingBuf.h"
#include <array>
#include <atomic>
#include <SPI.h>
//#include <SensorDefinitions.h>
//#include <SensorClass.h>
//#include <vector> 


// Use Teensy SDIO
#define SD_CONFIG  SdioConfig(FIFO_SDIO)

// Interval between points for 25 ksps.
#define LOG_INTERVAL_USEC 40

// Size to log 10 byte lines at 25 kHz for more than ten minutes.
#define LOG_FILE_SIZE 100*25000*600  // 1,500,000,000 bytes.

// Space to hold more than 800 ms of data for 10 byte lines at 25 ksps.
#define RING_BUF_CAPACITY 100*512
#define LOG_FILENAME "SdioLogger5.csv"

SdFs sd;
FsFile file;

// RingBuf for File type FsFile.
RingBuf<FsFile, RING_BUF_CAPACITY> rb;

void logDataStartup() {
  // Initialize the SD.
  if (!sd.begin(SD_CONFIG)) {
    sd.initErrorHalt(&Serial);
  }
  // Open or create file - truncate existing file.
  if (!file.open(LOG_FILENAME, O_RDWR | O_CREAT | O_TRUNC)) {
    Serial.println("open failed\n");
    return;
  }
  // File must be pre-allocated to avoid huge
  // delays searching for free clusters.
  if (!file.preAllocate(LOG_FILE_SIZE)) {
     Serial.println("preAllocate failed\n");
     file.close();
     return;
  }
  // initialize the RingBuf.
  rb.begin(&file);
  //Serial.println("Type any character to stop");

  // Max RingBuf used bytes. Useful to understand RingBuf overrun.
  // size_t maxUsed = 0;

/*   // Min spare micros in loop.
  int32_t minSpareMicros = INT32_MAX;

  // Start time.
  uint32_t logTime = micros(); */
}
  // Log data until Serial input or file full.
  /* while (!Serial.available()) {
    // Amount of data in ringBuf.
    size_t n = rb.bytesUsed();
    if ((n + file.curPosition()) > (LOG_FILE_SIZE - 20)) {
      Serial.println("File full - quitting.");
      break;
    }
    if (n > maxUsed) {
      maxUsed = n;
    }
    if (n >= 512 && !file.isBusy()) {
      // Not busy only allows one sector before possible busy wait.
      // Write one sector from RingBuf to file.
      if (512 != rb.writeOut(512)) {
        Serial.println("writeOut failed");
        break;
      }
    }
  } */
    /* // Time for next point.
    logTime += LOG_INTERVAL_USEC;
    int32_t spareMicros = logTime - micros();
    if (spareMicros < minSpareMicros) {
      minSpareMicros = spareMicros;
    }
    if (spareMicros <= 0) {
      Serial.print("Rate too fast ");
      Serial.println(spareMicros);
      break;
    } */
/*     // Wait until time to log data.
    while (micros() < logTime) {} */

    // Read ADC0 - about 17 usec on Teensy 4, Teensy 3.6 is faster.
    /* uint16_t adc = analogRead(0);
    // Print spareMicros into the RingBuf as test data.
    rb.print(spareMicros);
    rb.write(',');
    // Print adc into RingBuf.
    rb.println(adc);
    if (rb.getWriteError()) {
      // Error caused by too few free bytes in RingBuf.
      Serial.println("WriteError");
      break;
    } */
  
/*   // Write any RingBuf data to file.
  rb.sync();
  file.truncate();
  file.rewind(); */
/*   // Print first twenty lines of file.
  
  for (uint8_t n = 0; n < 20 && file.available();) {
    int c = file.read();
    if (c < 0) {
      break;
    }
    Serial.write(c);
    if (c == '\n') n++;
  }
  Serial.print("fileSize: ");
  Serial.println((uint32_t)file.fileSize());
  Serial.print("maxBytesUsed: ");
  Serial.println(maxUsed);
  Serial.print("minSpareMicros: ");
  Serial.println(minSpareMicros);
  file.close(); */

/* void clearSerialInput() {
  for (uint32_t m = micros(); micros() - m < 10000;) {
    if (Serial.read() >= 0) {
      m = micros();
    }
  }
} */


CAN_message_t message0;
CAN_message_t message1;
elapsedMillis sinceGUIsend;

//loop iterator declarations
int i;
int j;
int k;

//The Pressure calculated
int Pressure;
//stupid bit math
int byte0;
int byte1;
int bitshift;
int value;

//This looks awful, but it creates arrays to store every possible CANID frame's bytes to then retransmit

int CANIDARRAYBytes[2048][8]; //array with positions for all possible CANIDs in 11 bit standard ID format for storing recent messages
bool CANIDARRAYLen[2048];

//array for using in the values converted from GUI
//This exists so that any value that doesn't get a conversion applied still gets sent to the Pi.
//That automates sending the state report frames, anything we didn't convert is passed as is.
//It also sends any frames that happen to not have been in our conversion code incidentally to the Pi for it's low rate logging code to still catch.
//This could be structured as ints instead of 2D array, but then you need to write CAN code to chop up the ints automatically for the sends.
int CANIDARRAYConvertedBytes[2048][8]; 

int counter = 0;

int MAXSENSORVALUE = 36;

float PtConversion[2048][2];


bool teensy_sd_enabled;
SDClass teensy_sd;
String sd_string = "can_log0.bin";


const int chipSelect = BUILTIN_SDCARD;

void setup()
{
    delay(2500);
    Serial.println("Initializing SD.");
    


Can0.begin(500000);
Can1.begin(500000);
//[Sensor ID]
//[][0] = M Value
//[][1] = B Value
// Dome Reg Fuel PT
PtConversion[79][0] = 0.0186;
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
PtConversion[59][1] = -128.58;


}

void loop()
{
  logDataStartup();
/// CAN read on CAN bus from prop stand/rocket
  Can0.read(message0);
    // Print read CAN messages into the SD card ring buffer
    rb.println(message0.timestamp);
    rb.print(message0.id);

    //rb.write(',');


  CANIDARRAYLen[message0.id] = message0.len;
  // Serial.print(message0.len);
  // Serial.print(" ");
  // Serial.print(CANIDARRAYLen[message0.id]);
  // Serial.println();
  // Serial.print(message0.id);
  // Serial.println();
  // Serial.print(message0.buf[0]);
  // Serial.println();
  // Serial.print(message0.buf[1]);
  // Serial.println();
  for (int i = 0; i < message0.len; i++)
  {
    CANIDARRAYBytes[message0.id][i] = message0.buf[i];
    CANIDARRAYConvertedBytes[message0.id][i] = message0.buf[i]; //fills second array to swap converted values over in later step
  
    rb.print(message0.buf[i]);

        if (rb.getWriteError()) {
      // Error caused by too few free bytes in RingBuf.
      Serial.println("WriteError");
      break;
      }
  }



/// CAN send on CAN bus to GUI, every 100 ms
  if (sinceGUIsend >= 1000) {
    Serial.println("nothing to see here");

  //STICK SENSOR CONVERSIONS HERE!!!!!!!! Only do it for GUI refreshes not continuously
  //Use the CANIDARRAYConvertedBytes array to overwrite the converted values onto, 
  //that way anything like state reports that we don't convert pass through.
  //output of conversions should break the value back into CAN frame byte format to write to the send array
  
  for (int i = 0; i < 2048; i++)
  {
    if (CANIDARRAYBytes[i][0] != 0){
      if (PtConversion[i][0] != 0){
    byte0 = CANIDARRAYBytes[i][0];
    byte1 = CANIDARRAYBytes[i][1];
    bitshift = byte0*256;
    value = bitshift+byte1;
    Pressure = value*PtConversion[i][0]+PtConversion[i][1];
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
  }
    
/////BELOW IS THE CANSEND CODE, as long as you pack the converted values into CANIDARRAYConvertedBytes it will handle everything
  for (int j = 0; j < 2048; j++)
  {
    if (CANIDARRAYLen[j] != 0) {

      // Serial.print(CANIDARRAYLen[j]);
      // Serial.println();
      // Serial.print(j);
      // Serial.println();
  
    
      message1.id = j;
      message1.len = 2; //CANIDARRAYLen[j];
      // Serial.print("Length CANID  ");
      // Serial.print(CANIDARRAYLen[j]);
      // Serial.println();
      // Serial.print("Length of message 1  bef  ");
      // Serial.print(message1.len);
      // Serial.println();
      for (int k = 0; k < message1.len; k++)
      { 
        //CANIDARRAYConvertedBytes[j][k] = message1.buf[k]; 
        message1.buf[k] = CANIDARRAYConvertedBytes[j][k];
        // Serial.print(CANIDARRAYConvertedBytes[j][k]);
        // Serial.println();
      }
      // Serial.print("Length of message 1  after  ");
      // Serial.print(message1.len);
      Serial.println(message1.id);
      Serial.print(message1.buf[0]);
      Serial.print(", ");
      Serial.print(message1.buf[1]);
      Can1.write(message1);
    }
  }
  counter = counter+1;
  Serial.println();
  Serial.print("GUI Frame Loop Counter: ");
  Serial.print(counter);
  Serial.println();
  sinceGUIsend = 0;  


  // Write any RingBuf data to file.
  rb.sync();
  file.truncate();
  file.rewind();
  //file.close();
  }




}