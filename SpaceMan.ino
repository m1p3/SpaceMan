/* SpaceMan 
 *  
 *  
 */



#include "ws2812.h"

#if defined(ARDUINO) && ARDUINO >= 100
  // No extras
#elif defined(ARDUINO) // pre-1.0
  // No extras
#elif defined(ESP_PLATFORM)
  #include "arduinoish.hpp"
#endif

const int DATA_PIN = 18; // Avoid using any of the strapping pins on the ESP32
const uint16_t NUM_PIXELS = 256;  // How many pixels you want to drive
uint8_t MAX_COLOR_VAL = 32; // Limits brightness

int pausetime = 500;

rgbVal *pixels;

void displayOff();
void rainbow(unsigned long, unsigned long);
void scanner(unsigned long, unsigned long);
void dumpDebugBuffer(int, char *);

void dumpDebugBuffer(int id, char * debugBuffer) {
  Serial.print("DEBUG: (");
  Serial.print(id);
  Serial.print(") ");
  Serial.println(debugBuffer);
  debugBuffer[0] = 0;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");
  if(ws2812_init(DATA_PIN, LED_WS2812B)) {
    Serial.println("Init FAILURE: halting");
    while (true) {};
  }
  #if DEBUG_WS2812_DRIVER
    dumpDebugBuffer(-2, ws2812_debugBuffer);
  #endif
  pixels = (rgbVal*)malloc(sizeof(rgbVal) * NUM_PIXELS);
  displayOff();
  #if DEBUG_WS2812_DRIVER
    dumpDebugBuffer(-1, ws2812_debugBuffer);
  #endif
  Serial.println("Init complete");
}

int passes = 0;
int MAX_PASSES = 10;

void show_SpaceMAN()
{

// Links
  ALL_LEDS(0,25,150,0,0); // Linke Seite bis Hals 
  ALL_LEDS(25,33,0,150,0); // Linker Arm
  ALL_LEDS(33,41,0,0,150); // Linkes Zwischenstück zwischen Arm und Halz  
  ALL_LEDS(41,45,150,150,0); // Linker Kopfbereich

//Rechts
  ALL_LEDS(45,51,150,150,150); // Rechter Kopfberech

  ALL_LEDS(51,56,0,150,150); // Rechtes Zwischenstück zwischen Arm und Halz  
  ALL_LEDS(56,58,120,0,150); // Rechter Arm
  ALL_LEDS(58,83,50,50,50); // Rechter Seite bis Hals 

// Mitte  
  ALL_LEDS(83,170,50,200,150); // Rechter Seite bis Hals 
  ALL_LEDS(170,262,130,150,150); // Rechter Seite bis Hals   
  
  ws2812_setColors(NUM_PIXELS, pixels); 

}

void ALL_LEDS(int X1, int X2, uint8_t r, uint8_t g, uint8_t b) {
  for (uint16_t i = X1; i < X2; i++) {
    pixels[i] = makeRGBVal(r, g, b);  
  }
}


void loop() {
  rainbow(0, 5000);
  scanner(0, 5000);
  displayOff();
}

void loop_FOR_DEBUG_TESTING() {
  for(uint16_t i=0; i<NUM_PIXELS; i++) {
    pixels[i] = makeRGBVal(1, 1, 1);
  }
  pixels[0] = makeRGBVal(2, 1, 3);
  pixels[1] = makeRGBVal(5, 4, 6);
  pixels[2] = makeRGBVal(8, 7, 9);
  ws2812_setColors(2, pixels);
  //ws2812_setColors(NUM_PIXELS, pixels);
  #if DEBUG_WS2812_DRIVER
    dumpDebugBuffer(passes, ws2812_debugBuffer);
  #endif
  delay(1);
  if (++passes >= MAX_PASSES) {
    while(1) {}
  }
}

void displayOff() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels[i] = makeRGBVal(0, 0, 0);
  }
  ws2812_setColors(2, pixels);
  //ws2812_setColors(NUM_PIXELS, pixels);
}

void scanner(unsigned long delay_ms, unsigned long timeout_ms) {
  int currIdx = 0;
  int prevIxd = 0;
  bool RUN_FOREVER = (timeout_ms == 0 ? true : false);
  unsigned long start_ms = millis();
  while (RUN_FOREVER || (millis() - start_ms < timeout_ms)) {
    pixels[prevIxd] = makeRGBVal(0, 0, 0);
    pixels[currIdx] = makeRGBVal(MAX_COLOR_VAL, MAX_COLOR_VAL, MAX_COLOR_VAL);;
    ws2812_setColors(NUM_PIXELS, pixels);
    prevIxd = currIdx;
    currIdx++;
    if (currIdx >= NUM_PIXELS) {
      currIdx = 0;
    }
    delay(delay_ms);
  }
}

void rainbow(unsigned long delay_ms, unsigned long timeout_ms)
{
  const uint8_t color_div = 4;
  const uint8_t anim_step = 1;
  const uint8_t anim_max = MAX_COLOR_VAL - anim_step;
  rgbVal color = makeRGBVal(anim_max, 0, 0);
  rgbVal color2 = makeRGBVal(anim_max, 0, 0);
  uint8_t stepVal = 0;
  uint8_t stepVal2 = 0;

  bool RUN_FOREVER = (timeout_ms == 0 ? true : false);
  unsigned long start_ms = millis();
  while (RUN_FOREVER || (millis() - start_ms < timeout_ms)) {
    color = color2;
    stepVal = stepVal2;
  
    for (uint16_t i = 0; i < NUM_PIXELS; i++) {
      pixels[i] = makeRGBVal(color.r/color_div, color.g/color_div, color.b/color_div);
  
      if (i == 1) {
        color2 = color;
        stepVal2 = stepVal;
      }
  
      switch (stepVal) {
        case 0:
        color.g += anim_step;
        if (color.g >= anim_max)
          stepVal++;
        break;
        case 1:
        color.r -= anim_step;
        if (color.r == 0)
          stepVal++;
        break;
        case 2:
        color.b += anim_step;
        if (color.b >= anim_max)
          stepVal++;
        break;
        case 3:
        color.g -= anim_step;
        if (color.g == 0)
          stepVal++;
        break;
        case 4:
        color.r += anim_step;
        if (color.r >= anim_max)
          stepVal++;
        break;
        case 5:
        color.b -= anim_step;
        if (color.b == 0)
          stepVal = 0;
        break;
      }
    }
  
    ws2812_setColors(NUM_PIXELS, pixels);
  
    delay(delay_ms);
  }
}
