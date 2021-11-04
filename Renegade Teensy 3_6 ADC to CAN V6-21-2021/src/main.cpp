//#include <SensorHeader.h>
#include <Arduino.h>
#include <ADC.h>
#include <ADC_util.h>
#include <FlexCAN.h>
#include <kinetis_flexcan.h>
#include <WireKinetis.h>
#include <Adafruit_MCP9808.cpp>
#include <string>
#include <list>
using std::string;

///// NODE DECLARATION!!!!!
int node = 3; //engine node = 2, prop node = 3

///// ADC /////
ADC* adc = new ADC();
#define PINS 25 
#define PINS_DIFF 2
uint8_t adc_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10,
                      A11, A12, A13, A14, A15, A16, A17, A18, A19, A20, A21, A22, A23, A24};
uint8_t adc_pins_diff[] = {A10, A11};


//Accursed way to do different sample rate sensor reads
bool input_enablediff[2] = {};
bool input_enable1[25] = {};
bool input_enable10[25] = {};
bool input_enable100[25] = {};
bool input_enable1000[25] = {};

elapsedMillis sinceRead1;
elapsedMillis sinceRead10;
elapsedMillis sinceRead100;
elapsedMillis sinceRead1000;
elapsedMillis sinceReadRTD;

///// LED /////
elapsedMillis sinceLED;
int led = 13;
bool ledstate = 0;

///// CAN /////
CAN_message_t message;
CAN_message_t rxmsg;
CAN_message_t extended;

int value = 0;
int pin = 0;
int counter = 0;



///// SENSORS /////
//SENSOR pt1 {"DomeRegFuel", "pt1000", 21, 10};
//SENSOR pt2 {"DomeRegLOX", "pt1000", 22, 10};
//SENSOR pt3 {"FuelTank", "pt1000", 23, 100};
//SENSOR pt4 {"LOXTank", "pt1000", 24, 100};
//SENSOR pt5 {"HiPress", "pt5000", 20, 100};
//SENSOR pt6 {"Chamber", "pt1000", 0, 1000};
//SENSOR load_cell1 {"Thrust Load Cell 1", "loadcell1000", 1, 100};
//SENSOR load_cell2 {"Thrust Load Cell 2", "loadcell1000", 2, 100};
//SENSOR load_cell3 {"Thrust Load Cell 3", "loadcell1000", 3, 100};
//SENSOR tc1 {"External Chamber Wall TC", "tcktype",10,25};
//SENSOR tc2 {"LOX Tank Lower TC", "tcktype",12,1};
//SENSOR tc3 {"LOX Tank Middle TC", "tcktype",10,1};
//SENSOR tc4 {"LOX Tank Upper TC", "tcktype",12,1};

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



///// SV Control /////
int SVenable_1_pin = 24;
int SVenable_2_pin = 25;
int SVenable_3_pin = 26;
int SVenable_4_pin = 27;

///// Temp Sensor for TC Cold Junction /////
/* Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
int roundedtemp; */

//-------------------------------------------------------//
void setup() {
  //assign which sample rate array to choose for each input enable
  if (node == 2) //Engine Node ADC pins to read sensors on
    {
    input_enablediff[0] = 0;        //TC
    input_enablediff[1] = 0;        //TC
    input_enable100[0] = 1;         //LC1
    input_enable100[1] = 1;         //LC2
    input_enable100[2] = 1;         //LC2
    input_enable1000[6] = 1;        //Chamber1 PT
    input_enable1000[5] = 1;        //Chamber2 PT
    input_enable100[7] = 1;          //Fuel Inlet Prop Side PT
    input_enable10[8] = 1;         //Fuel Injector PT
    input_enable10[9] = 1;         //Lox Inlet Prop Side PT
    } else if (node == 3)  //Prop Node ADC pins to read sensors on
    {
    input_enablediff[0] = 0;        //TC
    input_enablediff[1] = 0;        //TC
    input_enable1[3] = 0;           //Main Valve Pneumatic PT
    input_enable10[4] = 0;          //Fuel Dome Reg PT
    input_enable10[5] = 0;          //Lox Dome Reg PT
    input_enable100[6] = 0;         //Fuel Tank PT
    input_enable100[7] = 0;         //Lox Tank PT
    input_enable100[8] = 0;         //Fuel High Press PT
    input_enable100[9] = 1;         //Lox High Press PT
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

  adc->adc0->setAveraging(32);                                    // set number of averages
  adc->adc0->setResolution(16);                                   // set bits of resolution
  adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS); // change the conversion speed
  adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);     // change the sampling speed
  //adc->adc0->recalibrate();

///// ADC1 /////
  adc->adc1->setAveraging(32);                                    // set number of averages
  adc->adc1->setResolution(16);                                   // set bits of resolution
  adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS); // change the conversion speed
  adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);     // change the sampling speed
  //adc->adc1->recalibrate();

  delay(500);

///// CAN0 and CAN1 Initialize /////
  Can0.begin(500000);
  Can1.begin(500000);
  pinMode(led, OUTPUT);


///// SV Control /////

pinMode(24, OUTPUT); //LOW drives it high to enable driver
pinMode(25, OUTPUT); //LOW drives it high to enable driver
pinMode(26, OUTPUT); //LOW drives it high to enable driver
pinMode(27, OUTPUT); //LOW drives it high to enable driver



/* ///// Temp Sensor for TC Cold Junction /////
tempsensor.begin(0x19);
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
 */
}

void loop() 
{
  //LED status indicator to show program is running
  if (sinceLED >= 50) {       //sets the if loop to only run if at least the number given milliseconds have passed
    if (ledstate == true)
    {
      digitalWrite(led, 0);
      ledstate = false;
    } else {
      digitalWrite(led, 1);
      ledstate = true;
    }
  }

  ///// Temp Sensor for TC Cold Junction Only/////
/*   if (sinceReadRTD >= 130) {       //sets the if loop to only run if at least the number given milliseconds have passed
    tempsensor.shutdown_wake(0);
    float c = tempsensor.readTempC();
    roundedtemp = int(c + 0.5);
    tempsensor.shutdown_wake(1);  //Not sure if this should correctly go here or not based on demo code.
    //Serial.print("Cold Junction Temp (C)");
    //Serial.print(c, 4);
    message.buf[0] = (roundedtemp >> 8) & 0xff;
    message.buf[1] = roundedtemp & 0xff;
    //message.buf[2] = (roundedtemp >> 8) & 0xff;
    //message.buf[3] = roundedtemp & 0xff;
    // message.buf[4] = (roundedtemp >> 8) & 0xff;
    // message.buf[5] = roundedtemp & 0xff;
    // message.buf[6] = (roundedtemp >> 8) & 0xff;
    // message.buf[7] = roundedtemp & 0xff;
    message.len = 2;  // number of bytes the message length uses
    message.id = 666 + (25 + node); // 11 bit ID, any value 0-2047, match ID for sensor on RPi receive end.
    Can0.write(message); //message send
    message.buf[1]++; //This is how the example library code did it, not totally sure why this is here
  sinceReadRTD = 0; //resets timer to zero each time the ADC is read
  } */

  if (sinceRead1 >= 1000) {       //sets the if loop to only run if at least the number given milliseconds have passed
    for (int i = 0; i < PINS; i++)
      if (input_enable1[i] == true) {
        {value = adc->analogRead(adc_pins[i]);
            Serial.print(" A");
            Serial.print(i);
            Serial.print(": ");
            Serial.print(value);
            Serial.println();
            // CAN buffers refer to each byte in a CAN frame based on how the CAN library works, up to 8. 
            // Below we take a 16 bit ADC read and split it to buffer 0 and buffer 1
            message.buf[0] = (value >> 8) & 0xff;
            message.buf[1] = value & 0xff;
            // message.buf[2] = (value >> 8) & 0xff;
            // message.buf[3] = value & 0xff;
            // message.buf[4] = (value >> 8) & 0xff;
            // message.buf[5] = value & 0xff;
            // message.buf[6] = (value >> 8) & 0xff;
            // message.buf[7] = value & 0xff;
            message.len = 2;  // number of bytes the message length uses
            message.id = i + (25 * node); // 11 bit ID, any value 0-2047, match ID for sensor on RPi receive end. Using ADC ID here.
            Can0.write(message); //message send
            message.buf[1]++; //This is how the example library code did it, not totally sure why this is here
        }  
    }
  sinceRead1 = 0; //resets timer to zero each time the ADC is read
  }
  if (sinceRead10 >= 100) {       //sets the if loop to only run if at least the number given milliseconds have passed
    for (int i = 0; i < PINS; i++)
      if (input_enable10[i] == true) {
        {value = adc->analogRead(adc_pins[i]);
            Serial.print(" A");
            Serial.print(i);
            Serial.print(": ");
            Serial.print(value);
            Serial.println();
            // CAN buffers refer to each byte in a CAN frame based on how the CAN library works, up to 8. 
            // Below we take a 16 bit ADC read and split it to buffer 0 and buffer 1
            message.buf[0] = (value >> 8) & 0xff;
            message.buf[1] = value & 0xff;
            // message.buf[2] = (value >> 8) & 0xff;
            // message.buf[3] = value & 0xff;
            // message.buf[4] = (value >> 8) & 0xff;
            // message.buf[5] = value & 0xff;
            // message.buf[6] = (value >> 8) & 0xff;
            // message.buf[7] = value & 0xff;
            message.len = 2;  // number of bytes the message length uses
            message.id = i + (25 * node); // 11 bit ID, any value 0-2047, match ID for sensor on RPi receive end. Using ADC ID here.
            Can0.write(message); //message send
            message.buf[1]++; //This is how the example library code did it, not totally sure why this is here
        }  
    }
    for (int i = 0; i < PINS_DIFF; i++) //TC READS
      if (input_enablediff[i] == true) {
        {value = adc->analogRead(adc_pins[i]);
            Serial.print(" A");
            Serial.print(i);
            Serial.print(": ");
            Serial.print(value);
            Serial.println();
            // CAN buffers refer to each byte in a CAN frame based on how the CAN library works, up to 8. 
            // Below we take a 16 bit ADC read and split it to buffer 0 and buffer 1
            message.buf[0] = (value >> 8) & 0xff;
            message.buf[1] = value & 0xff;
            // message.buf[2] = (value >> 8) & 0xff;
            // message.buf[3] = value & 0xff;
            // message.buf[4] = (value >> 8) & 0xff;
            // message.buf[5] = value & 0xff;
            // message.buf[6] = (value >> 8) & 0xff;
            // message.buf[7] = value & 0xff;
            message.len = 2;  // number of bytes the message length uses
            message.id = (i+10) * (25 * node); // 11 bit ID, any value 0-2047, match ID for sensor on RPi receive end. Using ADC ID here.
            Can0.write(message); //message send
            message.buf[1]++; //This is how the example library code did it, not totally sure why this is here
        }  
    }  
  sinceRead10 = 0; //resets timer to zero each time the ADC is read
  }

    if (sinceRead100 >= 10) {       //sets the if loop to only run if at least the number given milliseconds have passed
    for (int i = 0; i < PINS; i++)
      if (input_enable100[i] == true) {
        {value = adc->analogRead(adc_pins[i]);
            Serial.print(" A");
            Serial.print(i);
            Serial.print(": ");
            Serial.print(value);
            Serial.println();
            // CAN buffers refer to each byte in a CAN frame based on how the CAN library works, up to 8. 
            // Below we take a 16 bit ADC read and split it to buffer 0 and buffer 1
            message.buf[0] = (value >> 8) & 0xff;
            message.buf[1] = value & 0xff;
            Serial.print(message.buf[0]);
            Serial.print(": ");
            Serial.print(message.buf[1]);
            // message.buf[2] = (value >> 8) & 0xff;
            // message.buf[3] = value & 0xff;
            // message.buf[4] = (value >> 8) & 0xff;
            // message.buf[5] = value & 0xff;
            // message.buf[6] = (value >> 8) & 0xff;
            // message.buf[7] = value & 0xff;
            message.len = 2;  // number of bytes the message length uses
            message.id = i + (25 * node); // 11 bit ID, any value 0-2047, match ID for sensor on RPi receive end. Using ADC ID here.
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
            Serial.print(" A");
            Serial.print(i);
            Serial.print(": ");
            Serial.print(value);
            Serial.println();
            // CAN buffers refer to each byte in a CAN frame based on how the CAN library works, up to 8. 
            // Below we take a 16 bit ADC read and split it to buffer 0 and buffer 1
            message.buf[0] = (value >> 8) & 0xff;
            message.buf[1] = value & 0xff;
            // message.buf[2] = (value >> 8) & 0xff;
            // message.buf[3] = value & 0xff;
            // message.buf[4] = (value >> 8) & 0xff;
            // message.buf[5] = value & 0xff;
            // message.buf[6] = (value >> 8) & 0xff;
            // message.buf[7] = value & 0xff;
            message.len = 2;  // number of bytes the message length uses
            message.id = i + (25 * node); // 11 bit ID, any value 0-2047, match ID for sensor on RPi receive end. Using ADC ID here.
            Can0.write(message); //message send
            message.buf[1]++; //This is how the example library code did it, not totally sure why this is here
        }  
    }
  sinceRead1000 = 0; //resets timer to zero each time the ADC is read
  }

// SV duty cycle and driver chip testing
  //digitalWrite(24, HIGH);
  //digitalWrite(25, HIGH);
  //digitalWrite(26, HIGH);
  //digitalWrite(27, HIGH);

  //analogWrite(2, 256);      //High Press
  //analogWrite(5, 256);      //High Vent
/*   analogWrite(6, 256);
  analogWrite(7, 256);
  analogWrite(8, 256);
  analogWrite(9, 256); */

  //delay(25);

  //analogWrite(2, 125);
  //analogWrite(5, 125);
/*   analogWrite(6, 51);
  analogWrite(7, 51);
  analogWrite(8, 51);
  analogWrite(9, 55); 

  delay(250); */

  analogWrite(2, 0);
  analogWrite(5, 0);
  analogWrite(6, 0);
  analogWrite(7, 0);
  analogWrite(8, 0);
  analogWrite(9, 0); 
  //delay(3000);

}