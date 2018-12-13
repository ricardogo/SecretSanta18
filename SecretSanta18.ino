#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

/*
 * Mic related stuff
 */

#define NOISE 50 // this is the number of samples 

#define STATE_NOISE 0
#define STATE_LISTENING 1

const int micPin = A0;  // Analog input pin that the potentiometer is attached to

int noiseArray[NOISE];
int noiseCounter = 0;
int noiseAvg = 0;

/*
 * Lights related stuff
 */

#define lightPin 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, lightPin, NEO_GRB + NEO_KHZ800);


/*
 * App related stuff
 */

int state = STATE_NOISE;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  
  switch(state) {
    case STATE_NOISE:
    {
      noiseArray[noiseCounter] = analogRead(micPin);
      noiseCounter++;

      if(noiseCounter == NOISE) {
        
        for(int i = 0; i < NOISE; i++)
          noiseAvg += noiseArray[i];

        noiseAvg /= NOISE;
        
        Serial.print("Noise avg: ");
        Serial.println(noiseAvg);

        colorWipe(strip.Color(255, 0, 0), 50); // READY TO GO!
        state = STATE_LISTENING;
      }
      break;
    }
    case STATE_LISTENING:
    {
      int micValue = abs(analogRead(micPin) - noiseAvg);
      //Serial.println(micValue);
      
      int brightnessValue = constrain(map(micValue, 0, 80, 0, 100), 0, 100);
      Serial.println(brightnessValue);

      if(brightnessValue > 80) // if you're having issues with the lights not triggering, mess about with this value (between 0~100)
        rainbow(); // not happy with the animation? change it here!
      else
        colorShow(strip.Color(0, 0, 0)); // random(0,255)*brightnessValue/100
      break;
    }
  }

  delay(20);
}

void rainbow() {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
  }
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void colorShow(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++)
    strip.setPixelColor(i, c);

  strip.show();
}
