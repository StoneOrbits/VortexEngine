#include <FastLED.h>

#define DATA_PIN    8 // Replace with your actual pin number
#define NUM_LEDS    20   // Assuming you have only one LED
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define MOSFET_PIN 18

CRGB leds[NUM_LEDS];

void setup() {  
  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, HIGH);
  pinMode(DATA_PIN, OUTPUT);
  digitalWrite(DATA_PIN, LOW);
  
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
}

int i = 0;
void loop() {
  i++;
  // Turn on all LEDs to red
  fill_solid(leds, NUM_LEDS, CRGB(i % 255, 0, 0));
  FastLED.show();
  delay(100);  // Experiment with different delay values
}
