#include <FastLED.h>

#define TRIGGER 3 // Arduino Pin --> HC-SR04 Trig
#define ECHO 2    // Arduino Pin --> HC-SR04 Echo
#define PULSE_TO_CM_DIVIDER 58 // from HC-SR04 User's Manual v1.0

#define NUM_LEDS 1
#define DATA_PIN 9
#define BUZZER 6

#define MEASURE_INTERVAL 20 // milliseconds a measurement should happen, max. HC-SR04 is 50 per second
#define MIN_DISTANCE_CM 100

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
  pinMode(BUZZER, OUTPUT);
  digitalWrite(TRIGGER, HIGH); //Signal abschalten
  digitalWrite(TRIGGER, HIGH);

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

unsigned long rounding_filter(unsigned long l)
{
  if (l % 10 > 5) return (l / 10) * 10 + 1;
  return (l / 10) * 10;
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

    distance = rounding_filter(distance);
    if (distance != last_distance && distance < MIN_DISTANCE_CM)
    {
      last_distance = distance;
      if (!signal_LED)
      {
        last_LED_anim = time;
      }
      signal_LED = true;
    }
  }

  if (signal_LED)
  {
    int delta = (time - last_LED_anim);
    if (delta > 2000)
    {
      signal_LED = false;
      leds[0] = CRGB::Black;
      noTone(BUZZER);
    }
    else if (delta > 0)
    {
      leds[0].r = 255 - abs(map(delta, 0, 2000, -255, 255));
      tone(BUZZER, 2000 + abs(map(delta, 0, 2000, -1000, 1000)));
    }
    FastLED.show();
  }
}




