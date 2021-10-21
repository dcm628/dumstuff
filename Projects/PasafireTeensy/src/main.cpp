#include <Arduino.h>
#include <FlexCAN.h>
#include <kinetis_flexcan.h>
#include <WireKinetis.h>
#include <ADC.h>
#include <ADC_util.h>
#include <array>
#include "CANRead.h"
#include "ControlCommands.h"

///// ADC /////
ADC* adc = new ADC();
#define PINS 23 //was default 25 but skipping two pins for I2C bus use?
#define PINS_DIFF 2
uint8_t adc_pins[] = {A0, A1, A2, A3, A6, A7, A8, A9, A10,
                      A11, A12, A13, A14, A15, A16, A17, A18, A19, A20, A21, A22, A23, A24};
uint8_t adc_pins_diff[] = {A10, A11};

bool input_enable[23] = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true};


///// CAN /////
int led = 13;
CAN_message_t msgOut;
CAN_message_t rxmsg;
CAN_message_t extended;

int value = 0;
int pin = 0;
int counter = 0;

///// SV Control /////
int SVenable_1_pin = 24;
int SVenable_2_pin = 25;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < PINS; i++)
    {
        pinMode(adc_pins[i], INPUT);
    }

  Serial.begin(9600);

///// ADC0 /////
  // reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 or ADC_REFERENCE::REF_EXT.
  //adc->setReference(ADC_REFERENCE::REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

  adc->adc0->setAveraging(4);                                    // set number of averages
  adc->adc0->setResolution(16);                                   // set bits of resolution
  adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS); // change the conversion speed
  adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED);     // change the sampling speed
  //adc->adc0->recalibrate();

///// ADC1 /////
  adc->adc1->setAveraging(4);                                    // set number of averages
  adc->adc1->setResolution(16);                                   // set bits of resolution
  adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS); // change the conversion speed
  adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED);     // change the sampling speed
  //adc->adc1->recalibrate();

  delay(500);

  Can0.begin(500000);
  pinMode(led, OUTPUT);
}

void loop() {
  for (int i = 0; i < PINS; i++)
    if (input_enable[i] == true) {
      {value = adc->analogRead(adc_pins[i]);
          Serial.print(" A:");
          Serial.print(i);
          Serial.println();
          Serial.print(" Raw:");
          Serial.print(value);
          Serial.print(" Converted:");
          Serial.print(value * 3.3 / adc->adc0->getMaxValue(), 2);
          Serial.println();
          counter = counter+1;
          Serial.print("counter: ");
          Serial.print(counter);
          Serial.println();
          
          msgOut.buf[0] = (value >> 8) & 0xff;
          msgOut.buf[1] = value & 0xff;

          extended.id = 1;
          msgOut.len = 2;
          msgOut.id = counter;
          Can0.write(msgOut);
          msgOut.buf[1]++;


      }  
    }

// SV duty cycle and driver chip testing
// analogWrite(2, 256);
// analogWrite(6, 256);
// delay(150);
// analogWrite(2, 51);
// analogWrite(6, 51);
// delay(2500);
// analogWrite(2, 0);
// analogWrite(6, 0);

}