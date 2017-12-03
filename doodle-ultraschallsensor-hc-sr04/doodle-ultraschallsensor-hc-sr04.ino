#include <FastLED.h>

#define TRIGGER 3 // Arduino Pin --> HC-SR04 Trig
#define ECHO 2    // Arduino Pin --> HC-SR04 Echo
#define PULSE_TO_CM_DIVIDER 58 // from HC-SR04 User's Manual v1.0

#define NUM_LEDS 1
#define DATA_PIN 9

#define MEASURE_INTERVAL 20 // milliseconds a measurement should happen, max. HC-SR04 is 50 per second

#define DEBUG 0

//--------------------------------


CRGB leds[NUM_LEDS];

unsigned long last_measured = 0;
unsigned long last_LED_anim = 0;
unsigned long time = 0;
int last_distance = 0;
bool signal_LED = false;

void setup()
{
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  digitalWrite(TRIGGER, HIGH); //Signal abschalten

  Serial.begin(57600);

  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  time = millis();
}

int getDistanceInCentimeter()
{
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(3);
  noInterrupts();
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  long duration = pulseIn(ECHO, HIGH);
  interrupts();

  long distance = duration / PULSE_TO_CM_DIVIDER;
  return (distance);
}

void loop()
{
  time = millis();

  if ((time - last_measured) > MEASURE_INTERVAL)
  {
    last_measured = time;
    int distance = getDistanceInCentimeter();
    if (DEBUG)
    {
      Serial.write("distance:");
      Serial.print(distance, DEC);
      Serial.write(" cm\n");
    }
    
    distance = (distance / 10) * 10;
    if (distance != last_distance)
    {
      last_distance = distance;
      signal_LED = true;
      last_LED_anim = time;
    }
  }

  if (signal_LED)
  {
    int delta = (time - last_LED_anim);
    if (delta > 2000)
    {
      signal_LED = false;
      leds[0] = CRGB::Black;
    }
    else if (delta > 0)
    {
      int val = 255 - abs(map(delta, 0, 2000, -255, 255));
      leds[0].r = val;
    }
    FastLED.show();
  }
}



