# sl-ar-esp32-c6-zigbee

Experimentating with ESP32-C6-Zero for connectivity via Zigbee to HomeAssistant

## HomeAssistant
On the HomeAssistant side:
- the version used for the tests is 2025.11.1
- the Zigbee coordinator is a SLZB06 connected via ethernet (no wifi) 

The choice of the Zigbee coordinator was originally made (long ago) because of the issue with the noisy USB3.0 connections on Raspberry 4. 


## Notes common to all projects

(unless specified otherwise)

All experimentations are made using small (original) ESP32-C6-Zero or ESP32-C6-Zero-M from WaveShare.

The LED GPIO is usually referred to as RGB_BUILTIN in the scripts. For sketches that are expected to work with a monochrome led, we use the predefined LED_PIN. Just in case, the LED\_PIN is digital io 8.



Unless specified otherwise:
- all examples are Zigbee end points, even if the original example configure the device as a coordinator.
- the sketches contain a call to 'Zigbee.factoryReset();' usually linked to 3sec press on the boot button while operating. This will cause the device to attempt pairing.


Notes
Any changes to the configuration of the device (adding or removing a cluster, renaming items, ...) require pairing again, it is not enough to use the 'reconfigure' function from HomeAssistant. Proceed as follow:

* in HomeAssistant, open the page of the device and select the 'Remove' option: HomeAssistant will forget the device.
* in most cases, the device will automatically enter pairing mode and you can use the 'Add device' function from the Zigbee page in HomeAssistant
* If anything goes wrong and the pairing does not start or fail, try the force the device in pairing by pressing the boot button (or any specific button indicated in the README.md of the script) to force pairing mode.


## List of projects


| Folder| Description | Notes |
|---|---|---|
|  sketch_20260909_Zigbee_On_Off_Light | Shows a light, a binary input, a binary output  |   |






## From the original documentation provided by Waveshare and Espressif:



### Using Arduino IDE



To get more information about the Espressif boards see \[Espressif Development Kits](https://www.espressif.com/en/products/devkits).



\* Before Compile/Verify, select the correct board: `Tools -> Board`.

\* Select the End device Zigbee mode: `Tools -> Zigbee mode: Zigbee ED (end device)`

\* Select Partition Scheme for Zigbee: `Tools -> Partition Scheme: Zigbee 4MB with spiffs`

\* Select the COM port: `Tools -> Port: xxx` where the `xxx` is the detected COM port.

\* Optional: Set debug level to verbose to see all logs from Zigbee stack: `Tools -> Core Debug Level: Verbose`.





### Resources



\* Official ESP32 Forum: \[Link](https://esp32.com)

\* Arduino-ESP32 Official Repository: \[espressif/arduino-esp32](https://github.com/espressif/arduino-esp32)

\* ESP32-C6 Datasheet: \[Link to datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c6\_datasheet\_en.pdf)

\* ESP32-H2 Datasheet: \[Link to datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-h2\_datasheet\_en.pdf)

\* Official ESP-IDF documentation: \[ESP-IDF](https://idf.espressif.com)




