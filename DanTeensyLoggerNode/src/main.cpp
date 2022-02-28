// Renegade CAN Data logger and SF Stand message interpreter
// Unfortunately by Dan

#include <Arduino.h>
#include <FlexCAN.h>
#include <SD.h>



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



//The Pressure calculated
int Pressure;
//stupid bit math
float byte0, byte1, bitshift, value;

//This looks awful, but it creates arrays to store every possible CANID frame's bytes to then retransmit

float CANIDARRAYBytes[2048][8]; //array with positions for all possible CANIDs in 11 bit standard ID format for storing recent messages
bool CANIDARRAYLen[2048];

//array for using in the values converted from GUI
//This exists so that any value that doesn't get a conversion applied still gets sent to the Pi.
//That automates sending the state report frames, anything we didn't convert is passed as is.
//It also sends any frames that happen to not have been in our conversion code incidentally to the Pi for it's low rate logging code to still catch.
//This could be structured as ints instead of 2D array, but then you need to write CAN code to chop up the ints automatically for the sends.
int CANIDARRAYConvertedBytes[2048][8]; 

int counter = 0;


float PtConversion[2048][2];





void setup()
{
    delay(2500);
    Serial.println("Initializing SD.");
    Can0.begin(500000);
    //Can1.begin(500000);
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
    logDataStartup();
}

void loop()
{
/// CAN read on CAN bus from prop stand/rocket
  if(Can0.read(message0)) {
      // Print read CAN messages into the SD card ring buffer
      String hexMsg = "";
      for(unsigned char i : message0.buf) {
          hexMsg += String(i, HEX);
      }

      String out = String(message0.timestamp) + "," + message0.id + "," + hexMsg;
      output_file.println(out);
      output_file.flush();
      Serial.println(out);
  }

  CANIDARRAYLen[message0.id] = message0.len;

  for (int i = 0; i < message0.len; i++)
  {
    CANIDARRAYBytes[message0.id][i] = message0.buf[i];
    CANIDARRAYConvertedBytes[message0.id][i] = message0.buf[i]; //fills second array to swap converted values over in later step
  


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
  }
    
/////BELOW IS THE CANSEND CODE, as long as you pack the converted values into CANIDARRAYConvertedBytes it will handle everything
  for (int j = 0; j < 2048; j++)
  {
    if (CANIDARRAYLen[j] != 0) {

      message1.id = j;
      message1.len = 2;
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
      //Can1.write(message1);
    }
  }
  counter = counter+1;
  Serial.println();
  Serial.print("GUI Frame Loop Counter: ");
  Serial.print(counter);
  Serial.println();
  sinceGUIsend = 0;  


  // Write any RingBuf data to file.
  //file.close();
  }




}