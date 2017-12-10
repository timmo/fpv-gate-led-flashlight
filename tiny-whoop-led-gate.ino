#include <FastLED.h>

#define PULSE_TO_CM_DIVIDER 58 // from HC-SR04 User's Manual v1.0

#define TRIGGER 3 // Arduino Pin --> HC-SR04 Trig
#define ECHO 2    // Arduino Pin --> HC-SR04 Echo
#define LED_PIN 6
#define BUZZER 4

#define NUM_LEDS 24

#define MEASURE_INTERVAL 20 // milliseconds a measurement should happen, max. HC-SR04 is 50 per second
#define MIN_DISTANCE_CM 20
#define MAX_DISTANCE_CM 40

#define FLY_THROUGH_GRACE_PERIOD 1000

#define DEBUG 1

//--------------------------------


CRGB leds[NUM_LEDS];

unsigned long last_measured = 0;
unsigned long flyThroughAt = 0;
unsigned long time = 0;
int last_distance = 0;
bool flyThroughDetected = false;

void setup()
{
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(TRIGGER, HIGH); //Signal abschalten
  digitalWrite(TRIGGER, HIGH);

  Serial.begin(57600);

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
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

void makeNoise() {
  int delta = (time - flyThroughAt);
  if (delta > 500)
  {
    // flyThroughDetected = false;
    noTone(BUZZER);
  }
  else if (delta > 0)
  {
    tone(BUZZER, 2000 + abs(map(delta, 0, 500, -1000, 1000)));
  }

}

void makeLight() {
  int delta = (time - flyThroughAt);
  if (delta > 500)
  {
    fill_solid(leds, NUM_LEDS, CHSV( 190, 255, 125));
    FastLED.show();
  }
  else if (delta > 400)
  {
    int value = abs(map(delta, 400, 500, 0, 125));
    fill_solid(leds, NUM_LEDS, CHSV( 190, 255, value));
    FastLED.show();
  }
  else if (delta > 100)
  {
    int value = 255 - abs(map(delta, 100, 400, -255, 255));
    fill_solid(leds, NUM_LEDS, CHSV( 0, 255, value));
    FastLED.show();
  }
  else if (delta > 0)
  {
    int value = abs(map(delta, 0, 100, 125, 0));
    fill_solid(leds, NUM_LEDS, CHSV( 190, 255, value));
    FastLED.show();
  }
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
      //      Serial.write("distance:");
      //      Serial.print(distance, DEC);
      //      Serial.write(" cm\n");
      for (int i = 1; i <= 80; i++) {
        if (i == MAX_DISTANCE_CM || i == MIN_DISTANCE_CM) {
          Serial.write("+");
        }
        else if (i % 10 == 0) {
//          Serial.print(i, DEC);
          Serial.write("||");
        }
        else if (i % 5 == 0) {
          Serial.write("|");
        }
        else if (i <= distance) {
          Serial.write("-");
        }
        else {
          Serial.write(" ");
        }
      }
      Serial.write("\n");
    }

    //    distance = rounding_filter(distance);
    //    if (distance != last_distance && distance < MIN_DISTANCE_CM)
    if (MIN_DISTANCE_CM < distance && distance < MAX_DISTANCE_CM)
    {
      //      last_distance = distance;
      if (!flyThroughDetected)
      {
        flyThroughAt = time;
      }
      flyThroughDetected = true;
    }
  }

  if (flyThroughDetected)
  {
    int delta = (time - flyThroughAt);
    if (delta > FLY_THROUGH_GRACE_PERIOD)
    {
      flyThroughDetected = false;
    }
  }

  makeLight();
  makeNoise();

}




