#include <Adafruit_NeoPixel.h> 


#define NUM_LEDS 1
#define LED_CONTROL_PIN  8 //LED_BUILTIN

unsigned long toggle_vis_at = 0;
bool led_visible = true;

Adafruit_NeoPixel pixels (NUM_LEDS, LED_CONTROL_PIN, NEO_RGB + NEO_KHZ800);

uint8_t min8(uint8_t a, uint8_t b) {if (a<b) return a; else return b;}

/************************* helper: handle display of led color */
class handle_led{
 public: 
  handle_led();
  void show_for(uint32_t duration);
  void show_until(uint32_t deadline);
  void set_intensity_percentage(uint8_t percentage);
  uint8_t get_intensity();
  void set_intensity(uint8_t value);
  bool isVisible();
  private:
    uint8_t maxIntensity = 255;
    uint8_t avgIntensity = 127;
    uint32_t visible_until = 0;
    int8_t intensity = 0;
};

handle_led::handle_led(){
  visible_until = 0;
  intensity = avgIntensity;
}

void handle_led::show_for(uint32_t duration){
  visible_until = millis() + duration;
}

void handle_led::show_until(uint32_t deadline){
  visible_until = deadline;
}

void handle_led::set_intensity_percentage(uint8_t percentage){
  float temp = min8(100, percentage) * maxIntensity;
  intensity = temp / 100;
}

void handle_led::set_intensity(uint8_t value){
  intensity = value;
}

uint8_t handle_led::get_intensity(){
  return intensity;
}


bool handle_led::isVisible(){
  return visible_until >= millis();
}

handle_led red_led = handle_led();
handle_led green_led = handle_led();
handle_led blue_led = handle_led();

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

  toggle_vis_at = 0;
}

void show_red_led(uint32_t duration, uint8_t percentage){
  red_led.show_for(duration);
  red_led.set_intensity_percentage(percentage);
}

void show_red_led(uint32_t duration){
  red_led.show_for(duration);
  red_led.set_intensity(127);
}

void show_green_led(uint32_t duration){
  green_led.show_for(duration);
  green_led.set_intensity(127);
}


void show_blue_led(uint32_t duration){
  blue_led.show_for(duration);
  blue_led.set_intensity(127);
}


void  refresh_led_now(){
  uint16_t redchannel = 0;
  uint16_t greenchannel = 0;
  uint16_t bluechannel = 0;
 
  if (red_led.isVisible() || green_led.isVisible() || blue_led.isVisible()){ 

    if (toggle_vis_at < 1) {toggle_vis_at = millis()+500;led_visible=true;}

    if (toggle_vis_at < millis()) {toggle_vis_at = millis()+500;led_visible=!led_visible;}

    if (!led_visible) {clearWLed();return;}

    if (red_led.isVisible()) redchannel = red_led.get_intensity();
    if (blue_led.isVisible()) bluechannel = blue_led.get_intensity();
    if (green_led.isVisible()) greenchannel = green_led.get_intensity();
    
    setWLed(redchannel, greenchannel, bluechannel);

    return;
  } else {
    clearWLed();
    led_visible=false;
    return;
  }

} 


void WLed_loop() {
  
  refresh_led_now();

}