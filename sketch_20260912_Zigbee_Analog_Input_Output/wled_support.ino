#include <Adafruit_NeoPixel.h> 


#define NUM_LEDS 1
#define LED_CONTROL_PIN  8 //LED_BUILTIN

Adafruit_NeoPixel pixels (NUM_LEDS, LED_CONTROL_PIN, NEO_RGB + NEO_KHZ800);


void setWLed(uint16_t redColor, uint16_t greenColor ,uint16_t blueColor){
    pixels.setPixelColor(0, pixels.Color (redColor, greenColor, blueColor));
    pixels.show();
}

void clearWLed(){
    pixels.clear();
    pixels.show();
}


void WLed_setup(){
  // Init LED and turn it OFF (if LED_PIN == RGB_BUILTIN, the rgbLedWrite() will be used under the hood)
  pixels.begin();  
  pixels.clear();

}