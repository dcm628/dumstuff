// RocketDriver Propulsion Control and Data Acquisition - Embedded System Node Program
// Originally by Dan Morgan and Mat Arnold
// For Renegade, Pasafire, Beach Launch Team, and more
//
//

#include <Arduino.h>
#include <EEPROM.h>
#include <ADC.h>
#include <ADC_util.h>
#include <FlexCAN.h>
#include <kinetis_flexcan.h>
#include <WireKinetis.h>
#include <Wire.h>
#include "Adafruit_MCP9808.h"
#include <InternalTemperature.h>
#include <array>
#include <string>
#include <list>
using std::string;

#include "CANRead.h"
#include "CANWrite.h"
#include "CANReports.h"
#include "OperationFunctionTemplates.h"
#include "pinList.h"
#include "ValveDefinitions.h"
#include "PyroDefinitions.h"
#include "AutoSequenceDefinitions.h"

//Trying to figure out RTC stuff with these libs
/* #include <TimeLib.h>
#include <DS1307RTC.h> */

//For use in doing serial inputs as CAN commands for testing
uint8_t fakeCANmsg;

bool abortHaltFlag; //creates halt flag

///// NODE DECLARATION!!!!! /////
//default sets to max nodeID intentionally to be bogus until otherwise set
uint8_t nodeID;       //engine node = 2, prop node = 3, Pasafire node = 8
uint8_t nodeIDfromEEPROM;   //nodeID read out of EEPROM
bool nodeIDdetermine = 0;   //boolean flag for if startup is to run the nodeID detect read


///// ADC /////
ADC* adc = new ADC();
#define PINS 21 //full size is 25, but using pins for A18 and A19 for I2C bus
#define PINS_DIFF 4
uint8_t adc_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10,
                      A11, A12, A13, A14, A15, A20, A21, A22, A23, A24};
uint8_t adc_pins_diff[2][2] = {{A10, A12}, {A11, A13}};


//Accursed way to do different sample rate sensor reads
bool input_enablediff[2] = {};
bool input_enable1[23] = {};
bool input_enable10[23] = {};
bool input_enable100[23] = {};
bool input_enable1000[23] = {};

elapsedMillis sinceRead1;
elapsedMillis sinceRead10;
elapsedMillis sinceRead100;
elapsedMillis sinceRead1000;
elapsedMillis sinceReadRTD;

///// LED /////
elapsedMillis sinceLED;

bool ledstate = 0;

///// CAN /////
CAN_message_t message;
CAN_message_t rxmsg;
CAN_message_t extended;

int value = 0;
//int pin = 0;
int counter = 0;
int MCUtempPIN = 70;  //?? Not sure, I was trying to figure out how to read direct from the non Teensy MCU pin
int MCUtempraw;
int MCUtempCANID = 100; 

int busSpeed0 = 500000; //baudrate
int busSpeed1 = 500000; //baudrate

bool startup{true}; // bool for storing if this is the first loop on startup

uint32_t loopCount {0};// for debugging

// Set the global command, and global state
Command currentCommand{command_NOCOMMAND}; 
State currentState{State::passive};
State priorState;

//AutoSequence stuff for main
int64_t currentCountdownForMain;

// Set EEPROM address for storing states
uint8_t stateAddress{0};

// 2 timers to not print too much
//elapsedMillis sensorTimer;
//elapsedMillis timer2;


//Sensor --- ADC channel
/* Engine Node:
A0 - LC1
A1 - LC2
A2 - LC3
A5 - Chamber1 PT
A6 - Chamber2 PT
A7 - Fuel Inlet Prop side PT
A8 - Fuel Injector PT
A9 - Lox Inlet Prop side PT
Prop Node:
A3 - Main Valve Pneumatic PT
A4 - Fuel Dome Reg PT
A5 - Lox Dome Reg PT
A6 - Fuel Tank PT
A7 - Lox Tank PT
A8 - Fuel High Press PT
A9 - Lox High Press PT */



///// Temp Sensor for TC Cold Junction /////
//Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
int roundedtemp;


// -------------------------------------------------------------
// abort reset function -- NOT TESTED IN CURRENT VERSION, needs to be worked on
void abortReset()
{
  cli();
  EEPROM.update(stateAddress, static_cast<uint8_t>(State::abort)); // write abort code to the EEPROM to be read on restart
  sei();
  digitalWrite(pin::reset, 0);                                       // set reset pin low to restart
}




// -------------------------------------------------------------
/* void NodeIDDetect(uint8_t nodeID, bool startup, bool nodeIDdetermine, uint8_t nodeIDfromEEPROM)       //Function to run the nodeID hardware addressing
{
  if (startup)                          //only on startup assign the pins
    {
    pinMode(pin::NodeAddress0, INPUT);
    pinMode(pin::NodeAddress1, INPUT);
    pinMode(pin::NodeAddress2, INPUT);
    pinMode(pin::NodeAddress3, INPUT);
    delay(250);   //I want to get rid of this but previously I needed at least some delay before doing the detect to get a valid read
    }
  
  if (nodeIDdetermine)
    {
    //Read the four digital addressing pins
    uint8_t NodeAddressBit0 = digitalRead(pin::NodeAddress0);
    uint8_t NodeAddressBit1 = digitalRead(pin::NodeAddress1)<<1;
    uint8_t NodeAddressBit2 = digitalRead(pin::NodeAddress2)<<2;
    uint8_t NodeAddressBit3 = digitalRead(pin::NodeAddress3)<<3;
    //Use the read addresses to convert into an int for nodeID
    uint8_t NodeIDAddressRead;
    NodeIDAddressRead = NodeAddressBit0 + NodeAddressBit1 + NodeAddressBit2 + NodeAddressBit3;
    nodeID = NodeIDAddressRead;     //Setting the Global NodeID to detected NodeID
    }
  else
    nodeID = nodeIDfromEEPROM;
  return;

  //Need to tweak structure of passing in and returning nodeID
  //Need to implememt any behavior for start up, reset et cetera.
  //Do I need any delays? I did use one in the past but I'd like to eliminate or minimize it
} */
// -------------------------------------------------------------


//-------------------------------------------------------//
void setup() {
  
  //Read pin 28 as digital input, if pulled high this is node 2, if pulled low it's node 3
  //I would like to write this into EEPROM and check on startup with state to make this not able to flip on a power cycle mid test/launch and cutout the setup time

/*     pinMode(pin::NodeAddress0, INPUT);
    pinMode(pin::NodeAddress1, INPUT);
    pinMode(pin::NodeAddress2, INPUT);
    pinMode(pin::NodeAddress3, INPUT);
    delay(250); */
/*     if (digitalRead(pin::NodeAddress2) == 1) {
    nodeID = 2;
  } else {
    nodeID = 3;
  } */
  nodeID = 2;       //For manually assigning NodeID isntead of the address read, make sure to comment out for operational use
  startup = true;   // Necessary to set startup to true for the code loop so it does one startup loop for the state machine before entering regular loop behavior

  // ----- Hardware Abort Pin Setup ----- NOT CURRENTLY IN USE
  // This hardware abort allows us to command the Teensy to reboot itself by pulling the reset pin to ground
/*   pinMode(pin::reset, OUTPUT);
  digitalWrite(pin::reset, 1);
  pinMode(pin::abort, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(pin::abort), abortReset, RISING);  */

  // ----- Safety Pin Setup ----- // Should not be required anymore - state operations for valve enables should do it
  if (nodeID == 2) //Engine Node
    {
/*     pinMode(pin::HiPressHiVentSafe, OUTPUT);
    pinMode(pin::MainValvesSafe, OUTPUT);
    //digitalWrite(24, 1);
    //digitalWrite(25, 1);
    } else if (nodeID == 3)  //Prop Node
    {
    pinMode(pin::LoxDomeRegVentSafe, OUTPUT);
    pinMode(pin::FuelDomeRegVentSafe, OUTPUT);
    pinMode(pin::LoxVentSafe, OUTPUT);
    pinMode(pin::FuelVentSafe, OUTPUT); */
/*     digitalWrite(25, 1);
    digitalWrite(26, 1);
    digitalWrite(27, 1);
    digitalWrite(24, 1); */
    }




  // -----Read Last State off eeprom and update -----
  currentState = static_cast<State>(EEPROM.read(stateAddress));
  startupStateCheck(currentState, currentCommand);

  // -----Run Valve NodeID Check-----
  ValveNodeIDCheck(valveArray, nodeID);

  // -----Run Valve NodeID Check-----
  ValveEnableNodeIDCheck(valveEnableArray, nodeID);

  // -----Run Valve NodeID Check-----
  PyroNodeIDCheck(pyroArray, nodeID);

  // -----Run Valve Setup-----
  valveSetUp(valveArray);

  // -----Run ValveEnable Setup-----
  valveEnableSetUp(valveEnableArray);

  // -----Run Valve Setup-----
  pyroSetUp(pyroArray);

   // -----Run AutoSequence Setup-----
  autoSequenceSetUp(autoSequenceArray);



  //assign which sample rate array to choose for each input enable
/*   if (nodeID == 2) //Engine Node ADC pins to read sensors on
    {
    input_enablediff[0] = 1;        //TC
    input_enablediff[1] = 1;        //TC
    input_enable100[0] = 1;         //LC1
    input_enable100[1] = 1;         //LC2
    input_enable100[2] = 1;         //LC2
    input_enable1000[6] = 1;        //Chamber1 PT
    input_enable1000[5] = 1;        //Chamber2 PT
    input_enable100[7] = 1;          //Fuel Inlet Prop Side PT
    input_enable10[8] = 1;         //Fuel Injector PT
    input_enable10[9] = 1;         //Lox Inlet Prop Side PT
    } else if (nodeID == 3)  //Prop Node ADC pins to read sensors on
    {
    input_enablediff[0] = 1;        //TC
    input_enablediff[1] = 1;        //TC
    input_enable1[3] = 1;           //Main Valve Pneumatic PT
    input_enable10[4] = 1;          //Fuel Dome Reg PT
    input_enable10[5] = 1;          //Lox Dome Reg PT
    input_enable100[6] = 1;         //Fuel Tank PT
    input_enable100[7] = 1;         //Lox Tank PT
    input_enable100[8] = 1;         //Fuel High Press PT
    input_enable100[9] = 1;         //Lox High Press PT
    } */

    if (nodeID == 2) //Engine Node ADC pins to read sensors on
    {
    input_enablediff[0] = 0;        //TC
    input_enablediff[1] = 0;        //TC
    input_enable100[0] = 0;         //LC1
    input_enable100[1] = 0;         //LC2
    input_enable100[2] = 0;         //LC2
    input_enable1000[6] = 0;        //Chamber1 PT
    input_enable1000[5] = 0;        //Chamber2 PT
    input_enable100[7] = 0;          //Fuel Inlet Prop Side PT
    input_enable10[8] = 0;         //Fuel Injector PT
    input_enable10[9] = 0;         //Lox Inlet Prop Side PT
    } else if (nodeID == 3)  //Prop Node ADC pins to read sensors on
    {
    input_enablediff[0] = 0;        //TC
    input_enablediff[1] = 0;        //TC
    input_enable1[3] = 0;           //Main Valve Pneumatic PT
    input_enable10[4] = 0;          //Fuel Dome Reg PT
    input_enable10[5] = 0;          //Lox Dome Reg PT
    input_enable100[6] = 0;         //Fuel Tank PT
    input_enable100[7] = 0;         //Lox Tank PT
    input_enable100[8] = 0;         //Fuel High Press PT
    input_enable100[9] = 0;         //Lox High Press PT
    }
    

  // pin setup
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < PINS; i++)
    {
        pinMode(adc_pins[i], INPUT);
    }

  Serial.begin(9600);

///// ADC0 /////
  // reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 or ADC_REFERENCE::REF_EXT.
  //adc->setReference(ADC_REFERENCE::REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

  adc->adc0->setAveraging(8);                                    // set number of averages
  adc->adc0->setResolution(16);                                   // set bits of resolution
  adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS); // change the conversion speed
  adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);     // change the sampling speed
  //adc->adc0->recalibrate();

///// ADC1 /////
  adc->adc1->setAveraging(8);                                    // set number of averages
  adc->adc1->setResolution(16);                                   // set bits of resolution
  adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS); // change the conversion speed
  adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);     // change the sampling speed
  //adc->adc1->recalibrate();

  delay(500);

///// CAN0 and CAN1 Initialize /////
  Can0.begin(busSpeed0);
  //Can1.begin(busSpeed1); //commented out for Teensy3.5, also not current in use even on 3.6
  pinMode(pin::led, OUTPUT);


///// Temp Sensor for TC Cold Junction /////
/* Wire.setSDA(38);
Wire.setSCL(37); */

/* if(!tempsensor.begin(0x19))
{
  Serial.println("Temp Sensor did not initialize.");
}
else
{
//tempsensor.begin(0x19);
  //  A2 A1 A0 address
  //  0  0  0   0x18  this is the default address
  //  0  0  1   0x19
  //  0  1  0   0x1A
  //  0  1  1   0x1B
  //  1  0  0   0x1C
  //  1  0  1   0x1D
  //  1  1  0   0x1E
  //  1  1  1   0x1F
tempsensor.setResolution(2);
  // Match the fastest sample requests in the loop to the sample time for chosen setting or it will not return data
  // Mode Resolution SampleTime
  //  0    0.5°C       30 ms
  //  1    0.25°C      65 ms
  //  2    0.125°C     130 ms
  //  3    0.0625°C    250 ms
} */

  //timer2 = 0;

}

void loop() 
{
  //Display the node number with serial print statement start of each loop
/*   Serial.print("NodeID: ");
  Serial.println(node); */

  //LED status indicator to show program is running
  if (sinceLED >= 100) {       //sets the if loop to only run if at least the number given milliseconds have passed
    if (ledstate == true)
    {
      digitalWrite(pin::led, 0);
      ledstate = false;
    } else {
      digitalWrite(pin::led, 1);
      ledstate = true;
    }
  }

  // --- Read CAN bus and update current command ---
  if(CANread(Can0, currentCommand) && !startup) // do not execute on the first loop
  {
    Serial.print("Command Recieved: ");
    Serial.println(currentCommand);
  }

  while (Serial.available()) 
    {
    fakeCANmsg = Serial.read();
      if(fakeCANmsg  < command_SIZE) //enter 0 inter serial to trigger command read
      {
          //add in code here to prompt for command code and update current command from this
          //Serial.println("Enter Command Byte");
          //CurrentCommand = Serial.read();

              
              //if(fakeCANmsg < command_SIZE)                                           // this checks if the message at that location in the buffer could be a valid command
              //{
                  currentCommand = static_cast<Command>(fakeCANmsg);
              //}


          Serial.println("Command Entered");
                

        }
    }



  // -----Process Commands Here-----
  //currentCommand = command_vent;    //TESTING COMMAND INPUT ONLY
  commandExecute(currentState, priorState, currentCommand, valveArray, pyroArray, valveEnableArray, autoSequenceArray, abortHaltFlag);
    

  ////// ABORT FUNCTIONALITY!!!///// This is what overrides main valve and igniter processes! /////
  ////// DO NOT MOVE BEFORE "commandExecute" or after "valveTasks"/"pyroTasks"!!! /////
  haltFlagCheck(abortHaltFlag, valveArray, pyroArray, valveEnableArray);

  // -----Advance needed propulsion system tasks (valve, valve enables, pyro, autosequences) -----
  autoSequenceTasks(autoSequenceArray,nodeID);
  currentCountdownForMain = IgnitionAutoSequence.getCurrentCountdown();
  autoSequenceValveUpdate(valveArray, currentCountdownForMain);
  autoSequencePyroUpdate(pyroArray, currentCountdownForMain);  
  valveTasks(valveArray, nodeID);
  valveEnableTasks(valveEnableArray, nodeID);
  pyroTasks(pyroArray, nodeID);
  
  
  


/*     Serial.print("abortHaltFlag: ");
    Serial.println(abortHaltFlag); */


  // -----Update State on EEPROM -----
  cli(); // disables interrupts to protect write command
  EEPROM.update(stateAddress, static_cast<uint8_t>(currentState)); // Never use .write()
  sei(); // reenables interrupts after write is completed



  ///// Temp Sensor for TC Cold Junction Only/////
  /* if (sinceReadRTD >= 130) {       //sets the if loop to only run if at least the number given milliseconds have passed
    tempsensor.shutdown_wake(0);
    //float c = tempsensor.readTempC();
    int rawtemp = tempsensor.read16(MCP9808_REG_AMBIENT_TEMP);    //attempting to read raw temp bits to send, not preconverted to float
    //roundedtemp = int(c + 0.5);
    tempsensor.shutdown_wake(1);  //Not sure if this should correctly go here or not based on demo code.
    //Serial.print("Cold Junction Temp (C)");
    //Serial.print(c, 4);
    message.buf[0] = (rawtemp >> 8) & 0xff;
    message.buf[1] = rawtemp & 0xff;
    //message.buf[0] = (roundedtemp >> 8) & 0xff;
    //message.buf[1] = roundedtemp & 0xff;
    //message.buf[2] = (roundedtemp >> 8) & 0xff;
    //message.buf[3] = roundedtemp & 0xff;
    // message.buf[4] = (roundedtemp >> 8) & 0xff;
    // message.buf[5] = roundedtemp & 0xff;
    // message.buf[6] = (roundedtemp >> 8) & 0xff;
    // message.buf[7] = roundedtemp & 0xff;
    message.len = 2;  // number of bytes the message length uses
    message.id = 666 + (25 + nodeID); // 11 bit ID, any value 0-2047, match ID for sensor on RPi receive end.
    Can0.write(message); //message send
    //message.buf[1]++; //This is how the example library code did it, not totally sure why this is here
  sinceReadRTD = 0; //resets timer to zero each time the ADC is read
  }
 */
  if (sinceRead1 >= 1000) {       //sets the if loop to only run if at least the number given milliseconds have passed
    for (int i = 0; i < PINS; i++)
      if (input_enable1[i] == true) {
        {value = adc->analogRead(adc_pins[i]);
/*             Serial.print(" A");
            Serial.print(i);
            Serial.print(": ");
            Serial.print(value);
            Serial.println(); */
            // CAN buffers refer to each byte in a CAN frame based on how the CAN library works, up to 8. 
            // Below we take a 16 bit ADC read and split it to buffer 0 and buffer 1
            message.buf[0] = (value >> 8) & 0xff;
            message.buf[1] = value & 0xff;
            message.len = 2;  // number of bytes the message length uses
            message.id = i + (25 * nodeID); // 11 bit ID, any value 0-2047, match ID for sensor on RPi receive end. Using ADC ID here.
            Can0.write(message); //message send
            message.buf[1]++; //This is how the example library code did it, not totally sure why this is here
        }  
    }
  
  //CAN State Report Functions
  CAN2PropSystemStateReport(Can0, currentState, currentCommand, valveArray, pyroArray, valveEnableArray, abortHaltFlag, nodeID);
  CAN2AutosequenceTimerReport(Can0, autoSequenceArray, abortHaltFlag, nodeID);

  //Main Loop state and command print statements - for testing only
  Serial.print("currentState :");
  Serial.println(static_cast<uint8_t>(currentState));
  Serial.print("currentCommand :");
  Serial.println(currentCommand);

  
  sinceRead1 = 0; //resets timer to zero each time the ADC is read
  }
  if (sinceRead10 >= 100) {       //sets the if loop to only run if at least the number given milliseconds have passed
    for (int i = 0; i < PINS; i++)
      if (input_enable10[i] == true) {
        {value = adc->analogRead(adc_pins[i]);
/*             Serial.print(" A");
            Serial.print(i);
            Serial.print(": ");
            Serial.print(value);
            Serial.println(); */
            // CAN buffers refer to each byte in a CAN frame based on how the CAN library works, up to 8. 
            // Below we take a 16 bit ADC read and split it to buffer 0 and buffer 1
            message.buf[0] = (value >> 8) & 0xff;
            message.buf[1] = value & 0xff;
            message.len = 2;  // number of bytes the message length uses
            message.id = i + (25 * nodeID); // 11 bit ID, any value 0-2047, match ID for sensor on RPi receive end. Using ADC ID here.
            Can0.write(message); //message send
            message.buf[1]++; //This is how the example library code did it, not totally sure why this is here
        }  
    }
    for (int i = 0; i < PINS_DIFF/2; i++) //TC READS
      if (input_enablediff[i] == true) {
        {value = adc->analogReadDifferential(adc_pins_diff[0][i], adc_pins_diff[1][i]);
/*             Serial.print(" A");
            Serial.print(i);
            Serial.print(": ");
            Serial.print(value);
            Serial.println(); */
            // CAN buffers refer to each byte in a CAN frame based on how the CAN library works, up to 8. 
            // Below we take a 16 bit ADC read and split it to buffer 0 and buffer 1
            message.buf[0] = (value >> 8) & 0xff;
            message.buf[1] = value & 0xff;
            message.len = 2;  // number of bytes the message length uses
            message.id = (i+10) + (25 * nodeID); // 11 bit ID, any value 0-2047, match ID for sensor on RPi receive end. Using ADC ID here.
            Can0.write(message); //message send
            message.buf[1]++; //This is how the example library code did it, not totally sure why this is here
        }  
    } 

  MCUtempraw = InternalTemperature.readTemperatureC();
  //Serial.print(InternalTemperature.readTemperatureC(), 1);
/*   Serial.print(MCUtempraw, 1);
  Serial.println();
  message.buf[0] = (MCUtempraw >> 8) & 0xff;
  message.buf[1] = MCUtempraw & 0xff;
  message.len = 2;  // number of bytes the message length uses
  message.id = (MCUtempCANID * nodeID); // 11 bit ID, any value 0-2047, match ID for sensor on RPi receive end. Using ADC ID here.
  Can0.write(message); //message send */

  ///// SENDING VALVE STATES ///// - inside the 10 Hz sensor timer if section
  /* CANwrite(Can0, flagArray, 0);   // need IDs for different returns */

  //Serial.println("Do I even run the 10Hz loop?");
  //CAN2PropSystemStateReport(Can0, currentState, currentCommand, valveArray, pyroArray, valveEnableArray, abortHaltFlag, nodeID);
  sinceRead10 = 0; //resets timer to zero each time the ADC is read
  }


    if (sinceRead100 >= 10) {       //sets the if loop to only run if at least the number given milliseconds have passed
    for (int i = 0; i < PINS; i++)
      if (input_enable100[i] == true) {
        {value = adc->analogRead(adc_pins[i]);
            /* Serial.print(" A");
            Serial.print(i);
            Serial.print(": ");
            Serial.print(value);
            Serial.println(); */
            // CAN buffers refer to each byte in a CAN frame based on how the CAN library works, up to 8. 
            // Below we take a 16 bit ADC read and split it to buffer 0 and buffer 1
            message.buf[0] = (value >> 8) & 0xff;
            message.buf[1] = value & 0xff;
            //Serial.print(message.buf[0]);
            //Serial.print(": ");
            //Serial.print(message.buf[1]);
            message.len = 2;  // number of bytes the message length uses
            message.id = i + (25 * nodeID); // 11 bit ID, any value 0-2047, match ID for sensor on RPi receive end. Using ADC ID here.
            Can0.write(message); //message send
            message.buf[1]++; //This is how the example library code did it, not totally sure why this is here
        }  
    }
  sinceRead100 = 0; //resets timer to zero each time the ADC is read
  }

    if (sinceRead1000 >= 1) {       //sets the if loop to only run if at least the number given milliseconds have passed
    for (int i = 0; i < PINS; i++)
      if (input_enable1000[i] == true) {
        {value = adc->analogRead(adc_pins[i]);
/*             Serial.print(" A");
            Serial.print(i);
            Serial.print(": ");
            Serial.print(value);
            Serial.println(); */
            // CAN buffers refer to each byte in a CAN frame based on how the CAN library works, up to 8. 
            // Below we take a 16 bit ADC read and split it to buffer 0 and buffer 1
            message.buf[0] = (value >> 8) & 0xff;
            message.buf[1] = value & 0xff;
            message.len = 2;  // number of bytes the message length uses
            message.id = i + (25 * nodeID); // 11 bit ID, any value 0-2047, match ID for sensor on RPi receive end. Using ADC ID here.
            Can0.write(message); //message send
            message.buf[1]++; //This is how the example library code did it, not totally sure why this is here
        }  
    }
  sinceRead1000 = 0; //resets timer to zero each time the ADC is read
  }
startup = false;

/* digitalWrite(25, 1);
digitalWrite(26, 1);
digitalWrite(27, 1);
digitalWrite(24, 1);
digitalWrite(35, 1);
digitalWrite(36, 1); */


}