#ifndef CANREPORTSGUI_H
#define CANREPORTSGUI_H

#include <Arduino.h>
#include <FlexCAN.h>
#include <array>

#include "CANWrite.h"
//#include "StateList.h"
//#include "ControlFunctions.h"
#include <vector>

uint8_t engineNodeValveNum = 4;
uint8_t propNodeValveNum = 6;
uint8_t engineNodePyroNum = 2;
uint8_t propNodePyroNum = 0;
uint8_t vavleArrayCount;
uint8_t pyroArrayCount;

elapsedMicros PropSysReportTimer;
elapsedMicros AutoSequenceReportTimer;
elapsedMicros ValveReportTimer;




// Build a CAN report that iterates through and sends all new sensor data
void SensorArrayCANSend(FlexCAN& CANbus)
{
    static CAN_message_t msgOut;
    msgOut.ext = 0;
    msgOut.len = 2;
    uint32_t sensorValueToSend;
    
    for(auto sensor : sensorArray)
    {
        if (sensor->getNewSensorValueCheck())
        {
            msgOut.id = sensor->getSensorID();
            
            sensorValueToSend = sensor->getCurrentRawValue();
            msgOut.buf[0] = sensorValueToSend;
            msgOut.buf[1] = (sensorValueToSend >> 8);

            // write message to bus
            CANbus.write(msgOut);
            sensor->setNewSensorValueCheck(false);
/*             Serial.print("Sensor ID");
            Serial.print(msgOut.id);
            Serial.print("Sensor Value");
            Serial.println(sensorValueToSend); */
            {
                // add write error handling here, for now it does nothing
            }
        }
    }
}

/* bool CAN2ValveStateReport()
{
    




} */



////// CAN FD Upgrade Versions //////
// TBD //

/* bool CANFDPropSystemStateReport()
{

} */


/* bool CANFDValveStateReport()
{

} */

#endif