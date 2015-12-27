/** 
 * @author Alexander Entinger MSc, LXRobotics
 * @brief this file demonstrates how to use the LXR_CB_Gas_Sensor library in combination with an lxrobotics funkshield too form a wireless gas warning system network - Attention: This is not a calibrated nor a certified smoke/gas/whatever detector. Therefor it can never replace a "real" smoke/gas/.. warning system.
 * @file gas_warning_system_radio.ino
 * @license Attribution-NonCommercial-ShareAlike 3.0 Unported (CC BY-NC-SA 3.0) ( http://creativecommons.org/licenses/by-nc-sa/3.0/ )
 */

#include "LXR_CB_Gas_Sensor.h"
#include "RFM12B.h"
#include "avr/sleep.h"

#define NODEID        1  // Network ID of this unit -> Needs to be different for every node 0 to 127
#define NETWORKID    99  // Network ID
#define GATEWAYID     0  // BROADCAST

// Need an instance of the Radio Module
RFM12B radio;

void setup() {               
  // initialize the gas sensor childboard 
  LXR_CB_Gas_Sensor::begin();
  // turn on the gas sensor
  LXR_CB_Gas_Sensor::turn_on_sensor();
  // initialize radio
  radio.Initialize(NODEID, RF12_433MHZ, NETWORKID);
  // put radio to sleep to save power
  radio.Sleep();

}

static int const threshold = 300;

static uint8_t const alarm_on_payload = 0xAA;

void loop() {
  // check if there is gas in the room and if thats the case, send an alarm message
  if(LXR_CB_Gas_Sensor::read_gas_sensor() > threshold) {
    LXR_CB_Gas_Sensor::turn_on_led();
    LXR_CB_Gas_Sensor::turn_on_alarm();
    radio.Send(GATEWAYID, &alarm_on_payload, 1, false); // send the alarm message out
  } 

  // if a package was received, check if it was an alarm package
  if (radio.ReceiveComplete()) {
    if (radio.CRCPass()) {
      if(radio.GetDataLen() == 1) {
        if(radio.Data[0] == alarm_on_payload) {
          LXR_CB_Gas_Sensor::turn_on_led();
          LXR_CB_Gas_Sensor::turn_on_alarm();
        }
      }
    }
  }
  delay(100);
}


