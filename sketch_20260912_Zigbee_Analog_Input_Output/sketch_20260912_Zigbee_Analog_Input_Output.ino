#include <Zigbee.h>
#include <ZigbeeCore.h>
#include <ZigbeeEP.h>
#include <ZigbeeTypes.h>

#include <Adafruit_NeoPixel.h> 

#include <Arduino.h>
#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#define DURATION_SHOW_RESET_INDICATOR 3000
#define DURATION_SHOW_IDENTIFY_INDICATOR 3000

// #define NUM_LEDS 1
// #define LED_CONTROL_PIN  8 //LED_BUILTIN

// Adafruit_NeoPixel pixels (NUM_LEDS, LED_CONTROL_PIN, NEO_RGB + NEO_KHZ800);



/* Zigbee light bulb configuration */

#define ANALOG_DEVICE_ENDPOINT_NUMBER 1

#define COUNTER_RESET_PIN 20
#define PULSE_INPUT_PIN 21
#define BOOT_BUTTON_PIN 22 // BOOT_PIN

 

ZigbeeAnalog myZBAnalogDevicePulse = ZigbeeAnalog(ANALOG_DEVICE_ENDPOINT_NUMBER);
ZigbeeAnalog myZBAnalogDeviceTotal = ZigbeeAnalog(ANALOG_DEVICE_ENDPOINT_NUMBER+1);
ZigbeeAnalog myZBAnalogDeviceFlow   = ZigbeeAnalog(ANALOG_DEVICE_ENDPOINT_NUMBER+2);

uint32_t pulse_counter = 0;
uint32_t base_counter = 0;

uint32_t last_minute_counter = 0;
uint32_t last_minute_deadline = 0;
uint32_t last_flow_counter = 0;

bool pulse_indicator;

/********************** HANDLING ANALOG CHANGES *******************/
void onAnalogOutputChange(float analog_output) {

  update_base_counter(analog_output, true);

  Serial.printf("Received analog output change: %.1f\r\n", analog_output);
}

/********************** HANDLING IDENTIFY ****************************/
void handle_identify(uint16_t value) {
  show_green_led(DURATION_SHOW_IDENTIFY_INDICATOR);
  show_red_led(DURATION_SHOW_IDENTIFY_INDICATOR);
}

/********************** HANDLING INPUT ****************************/
void handle_pulse(bool state) {
  if(pulse_indicator != state) show_blue_led(500);
  pulse_indicator = state;
}

/********************* Update base counter ************************/
void update_base_counter(uint32_t new_value, bool save_to_file){
  base_counter = new_value;
  myZBAnalogDeviceTotal.reportAnalogOutput();

  myZBAnalogDeviceTotal.setAnalogInput(base_counter + pulse_counter);
  myZBAnalogDeviceTotal.reportAnalogInput();

  if (save_to_file) file_put_baseCounter(base_counter);

}

/********************** Update flow *******************************/
void update_flow(uint32_t last_value, uint32_t old_value){

  uint32_t temp = 0;

  if (old_value > last_value) temp = 0; else temp = last_value - old_value;

  if (last_flow_counter !=  temp) {
    last_flow_counter =  temp;

    myZBAnalogDeviceFlow.setAnalogInput(temp);
    myZBAnalogDeviceFlow.reportAnalogInput();
  }
}


/********************* Update pulse counter ***********************/
void update_pulse_counter(bool increment, uint32_t forced_value, bool save_to_file){

  if (increment) { pulse_counter += 1;} else {
    pulse_counter = forced_value;
  }
  myZBAnalogDevicePulse.setAnalogInput(pulse_counter);
  myZBAnalogDevicePulse.reportAnalogInput();

  myZBAnalogDeviceTotal.setAnalogInput(base_counter + pulse_counter);
  myZBAnalogDeviceTotal.reportAnalogInput();
 
  if (save_to_file)  file_put_currentCounter(pulse_counter);

}


/********************* Arduino functions **************************/
void setup() {
  Serial.begin(115200);

  WLed_setup();
  
  // Init application i/o
  pinMode(COUNTER_RESET_PIN, INPUT_PULLUP);
  pinMode(PULSE_INPUT_PIN, INPUT_PULLUP);

  // Init button for factory reset
  pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);
 
  files_setup();
  
  //Optional: set Zigbee device name and model
  myZBAnalogDevicePulse.setManufacturerAndModel("Kverzu-Home", "PulseCounter");


/************** Setup Analog Input, reporting pulse counter **************** */
  myZBAnalogDevicePulse.addAnalogInput();
  myZBAnalogDevicePulse.setAnalogInputApplication(ESP_ZB_ZCL_AI_COUNT_UNITLESS_OTHER);
  myZBAnalogDevicePulse.setAnalogInputDescription("Counter delta (l)");
  myZBAnalogDevicePulse.setAnalogInputResolution(1);

  myZBAnalogDevicePulse.onIdentify(&handle_identify);
  
/************** Setup Analog Input, reporting total **************** */
  myZBAnalogDeviceTotal.addAnalogInput();
  myZBAnalogDeviceTotal.setAnalogInputApplication(ESP_ZB_ZCL_AI_COUNT_UNITLESS_OTHER);
  myZBAnalogDeviceTotal.setAnalogInputDescription("Counter total (l)");
  myZBAnalogDeviceTotal.setAnalogInputResolution(1);
  
/************** Setup Analog outnput, receiving base counter value **************** */
  myZBAnalogDeviceTotal.addAnalogOutput();
  myZBAnalogDeviceTotal.setAnalogOutputApplication(ESP_ZB_ZCL_AI_COUNT_UNITLESS_OTHER);
  myZBAnalogDeviceTotal.setAnalogOutputDescription("Counter base (l)");
  myZBAnalogDeviceTotal.setAnalogOutputResolution(1);

/************** Setup Analog Input, reporting flow (l/min) **************** */
  myZBAnalogDeviceFlow.addAnalogInput();
  myZBAnalogDeviceFlow.setAnalogInputApplication(ESP_ZB_ZCL_AI_COUNT_UNITLESS_OTHER);
  myZBAnalogDeviceFlow.setAnalogInputDescription("Flow (l/min)");
  myZBAnalogDeviceFlow.setAnalogInputResolution(1);


  
  // Set the min and max values for the analog output which is used by HA to limit the range of the analog output
  myZBAnalogDeviceTotal.setAnalogOutputMinMax(0, 9999999);  

  // If analog output cluster is added, set callback function for analog output change
  myZBAnalogDeviceTotal.onAnalogOutputChange(onAnalogOutputChange);

  //Add endpoint to Zigbee Core
  Serial.println("Adding ZigbeeAnalogDevice endpoint to Zigbee Core");
  Zigbee.addEndpoint(&myZBAnalogDeviceTotal);
  Zigbee.addEndpoint(&myZBAnalogDevicePulse);
  Zigbee.addEndpoint(&myZBAnalogDeviceFlow);

  //*************************

  Serial.println("Starting Zigbee...");

  // When all EPs are registered, start Zigbee. By default acts as ZIGBEE_END_DEVICE
  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  }
  Serial.println("Connecting to network");

  show_red_led(99999);
  
  while (!Zigbee.connected()) {
    refresh_led_now();
    delay(100);
  }
  
  Serial.println();

  show_red_led(0);

  refresh_led_now();

  update_base_counter(file_get_baseCounter(), false );
  update_pulse_counter(true, file_get_currentCounter(), false );

  last_minute_counter = 0;
  last_minute_deadline = millis();
  last_minute_deadline +=  1000 * 60;
  
  Serial.println("Connected.");
}


bool last_button_state = false;

void loop() {
  bool button_state;

// ********************* Init or millis() back to zero ****************************** //
  if (last_minute_deadline == 0 || millis() < 500) {
    last_minute_deadline = millis() + 1000 * 60;
    last_minute_counter = pulse_counter;
  }

// ********************* deadline passed ****************************** //
  if (last_minute_deadline < millis()) {

    update_flow(pulse_counter, last_minute_counter);

    last_minute_deadline = millis() + 1000 * 60;
    last_minute_counter = pulse_counter;
  }

  // Checking control button for factory reset
  if (digitalRead(BOOT_BUTTON_PIN) == LOW) {  // Push button pressed
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(BOOT_BUTTON_PIN) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        // If key pressed for more than 3secs, factory reset Zigbee and reboot
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        Zigbee.factoryReset();
      }
    }
  }

  // Checking control button for counter reset
  if (digitalRead(COUNTER_RESET_PIN) == LOW) {  // Push button pressed
    // quick and dirty key debounce handling
    delay(100);
    if (digitalRead(COUNTER_RESET_PIN) == LOW) {
      update_pulse_counter(false, 0, true);
      show_green_led(DURATION_SHOW_RESET_INDICATOR);
    }
  }
  
  // Checking pulse input
  if (digitalRead(PULSE_INPUT_PIN) == LOW) {  // Push button pressed
      delay(100);
      if (digitalRead(PULSE_INPUT_PIN) == LOW) {
          button_state = (digitalRead(PULSE_INPUT_PIN) == LOW);
          if (button_state != last_button_state) {
            update_pulse_counter(true, 0, true);
            last_button_state = button_state;
            handle_pulse(true);
          }

      }
  } else {
    if (button_state != last_button_state) handle_pulse(false);
    last_button_state = false;
  }

  WLed_loop();
  delay(100);

}