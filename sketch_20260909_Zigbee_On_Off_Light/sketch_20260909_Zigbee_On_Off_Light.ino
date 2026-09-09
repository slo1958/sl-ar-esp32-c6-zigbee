// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @brief This example demonstrates simple Zigbee light bulb.
 *
 * The example demonstrates how to use Zigbee library to create a end device light bulb.
 * The light bulb is a Zigbee end device, which is controlled by a Zigbee coordinator.
 *
 * Proper Zigbee mode must be selected in Tools->Zigbee mode
 * and also the correct partition scheme must be selected in Tools->Partition Scheme.
 *
 * Please check the README.md for instructions and more detailed description.
 *
 * Created by Jan Procházka (https://github.com/P-R-O-C-H-Y/)
 */

#include <Arduino.h>
#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"

/* Zigbee light bulb configuration */
#define ZIGBEE_LIGHT_ENDPOINT 10
#define BINARY_DEVICE_ENDPOINT_NUMBER 1

#define USER_BUTTON_PIN 22
#define CTRL_BUTTON_PIN BOOT_PIN
#define LIGHT_PIN  LED_BUILTIN
 

ZigbeeLight  myZBLight = ZigbeeLight(ZIGBEE_LIGHT_ENDPOINT);
ZigbeeBinary myZBBinaryInput = ZigbeeBinary(BINARY_DEVICE_ENDPOINT_NUMBER);

/********************** HANDLING INPUT ****************************/
void handleSwitch(bool state) {
  // Copy the received binary output state to the binary input and report change to network
  Serial.println("Switch switch changed to: " + String(state));
 
  myZBBinaryInput.setBinaryInput(state);
  myZBBinaryInput.reportBinaryInput();

}

/********************* RGB LED functions **************************/
void handleLight(bool value) {
  digitalWrite(LIGHT_PIN, value);
}

/********************* Arduino functions **************************/
void setup() {
  Serial.begin(115200);

  // Init LED and turn it OFF (if LED_PIN == RGB_BUILTIN, the rgbLedWrite() will be used under the hood)
  pinMode(LIGHT_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, LOW);

  // Init user button
  pinMode(USER_BUTTON_PIN, INPUT_PULLUP);

  // Init button for factory reset
  pinMode(CTRL_BUTTON_PIN, INPUT_PULLUP);

  // ****************** Handling setup of myZBLight

  //Optional: set Zigbee device name and model
  myZBLight.setManufacturerAndModel("Espressif", "ZBLightBulb");

  // Set callback function for light change
  myZBLight.onLightChange(handleLight);

  //Add endpoint to Zigbee Core
  Serial.println("Adding ZigbeeLight endpoint to Zigbee Core");
  Zigbee.addEndpoint(&myZBLight);


  // ****************** Handling setup of myZBBinaryInput

  myZBBinaryInput.addBinaryInput();
  myZBBinaryInput.setBinaryInputApplication(BINARY_INPUT_APPLICATION_TYPE_HVAC_FAN_STATUS);
  myZBBinaryInput.setBinaryInputDescription("Status (I/P)");

  myZBBinaryInput.addBinaryOutput();
  myZBBinaryInput.setBinaryOutputApplication(BINARY_OUTPUT_APPLICATION_TYPE_HVAC_FAN);
  myZBBinaryInput.setBinaryOutputDescription("Switch (O/P)");

  myZBBinaryInput.onBinaryOutputChange(handleSwitch);

  //Add endpoint to Zigbee Core
  Serial.println("Adding ZigbeeBinaryInput endpoint to Zigbee Core");
  Zigbee.addEndpoint(&myZBBinaryInput);

  //*************************

  // When all EPs are registered, start Zigbee. By default acts as ZIGBEE_END_DEVICE
  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  }
  Serial.println("Connecting to network");
  while (!Zigbee.connected()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
}

bool last_button_state = false;

void loop() {
  // Checking control button for factory reset
  if (digitalRead(CTRL_BUTTON_PIN) == LOW) {  // Push button pressed
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(CTRL_BUTTON_PIN) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        // If key pressed for more than 3secs, factory reset Zigbee and reboot
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        Zigbee.factoryReset();
      }
    }
  }

  if (digitalRead(USER_BUTTON_PIN) == LOW) {  // Push button pressed
    // quick and dirty key debounce handling
    delay(100);
    }
  
  bool button_state = (digitalRead(USER_BUTTON_PIN) == LOW);
  bool zigbee_state = myZBBinaryInput.getBinaryOutput();

  //bool curstate = myZBLight.getLightState();

 
  if (button_state != last_button_state) {
    myZBLight.setLight(button_state); // !zbLight.getLightState());
    // myZBBinaryInput.setBinaryOutput(button_state);
    // myZBBinaryInput.reportBinaryOutput();
    last_button_state = button_state;
  }
    
  delay(100);
}
