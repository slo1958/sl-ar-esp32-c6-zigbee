#include <Adafruit_NeoPixel.h> 

// #include <Arduino.h>
// #ifndef ZIGBEE_MODE_ED
// #error "Zigbee end device mode is not selected in Tools->Zigbee mode"
// #endif

// #include "Zigbee.h"

#define ZIGBEE_RGB_LIGHT_ENDPOINT 10

#define LED_PIN  8
#define NUM_LEDS 1

#define CTRL_BUTTON_PIN 22
#define PULSE_INPUT_PIN 21
#define IOCHCK_INPUT_PIN 20

//
// On ESP32C6-Zero Red and green lines are swapped
Adafruit_NeoPixel pixels (NUM_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  // Initialize serial communication with baud rate 9600
  while (!Serial){
    ; // Wait for serial connection to establish
  };


  pinMode(CTRL_BUTTON_PIN, INPUT_PULLUP);  //
  pinMode(PULSE_INPUT_PIN, INPUT_PULLUP);  //
  pinMode(IOCHCK_INPUT_PIN, INPUT_PULLUP);  //

}

#define BUTTON_ON 10
#define BUTTON_OFF 11
#define BUTTON_UNDEFINED 12

#define BUTTON_IDLE 21
#define BUTTON_PRESSED 22
#define BUTTON_RELEASED 23
#define BUTTON_DORESET 27

uint8_t buttonState = BUTTON_UNDEFINED;
uint8_t buttonLastEvent = BUTTON_IDLE;
uint32_t buttonLastPressTime=0;

  // Current button state
int workBtnOnTime = 0;



void handleButtonLastEvent() {
  if (buttonLastEvent != BUTTON_DORESET) buttonLastEvent = BUTTON_IDLE;
}

 
void setButtonLastEvent(uint8_t theEvent) {
  if ( (buttonLastEvent == BUTTON_IDLE) || (theEvent == BUTTON_DORESET) ) {
     buttonLastEvent = theEvent;
  }
}

// The state is LOW wshen the button is pressed
void checkButtonState(){

  int workBtnState = digitalRead(CTRL_BUTTON_PIN);  // Read current button state

  if (buttonState == BUTTON_OFF && workBtnState == LOW) {
    buttonState = BUTTON_ON;
    setButtonLastEvent(BUTTON_PRESSED);
    buttonLastPressTime = millis();

  } else if(buttonState == BUTTON_ON && workBtnState == LOW) {
    if (buttonLastPressTime + 3000 < millis()) {
        setButtonLastEvent(BUTTON_DORESET);
    }

  } else if (buttonState == BUTTON_ON && workBtnState == HIGH) {
    buttonState = BUTTON_OFF;
    setButtonLastEvent(BUTTON_RELEASED);
 
  }
}

#define PIXEL_CHANNEL 0

void pixelblink(uint32_t color, uint32_t showdelay, uint16_t repeat){

  for (uint16_t wr = repeat ; wr>0; wr--){
    pixels.setPixelColor(PIXEL_CHANNEL, color);
    pixels.show();
    delay(showdelay);
    pixels.clear();
    delay(showdelay);
  }
}

void pixelshow(uint32_t color, uint16_t showdelay){
    pixels.setPixelColor(PIXEL_CHANNEL, color);
    pixels.show();
    delay(showdelay);
    pixels.clear();
}

void run_io_checks(){
  if (digitalRead(CTRL_BUTTON_PIN)==LOW) {pixelshow(pixels.Color(255, 0, 0), 333);}
  if (digitalRead(PULSE_INPUT_PIN)==LOW) {pixelshow(pixels.Color(0, 255, 0), 333);}
  pixelshow(pixels.Color(0, 0, 255), 333);
  delay(333);
}

void loop() {
  if (digitalRead(IOCHCK_INPUT_PIN) == LOW) {
    run_io_checks();
  } else {
    checkButtonState();
    handleButtonLastEvent(); 
  }
 
}
