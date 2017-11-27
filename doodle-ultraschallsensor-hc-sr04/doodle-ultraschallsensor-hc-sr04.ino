
#define TRIGGER 3 // Arduino Pin --> HC-SR04 Trig
#define ECHO 2    // Arduino Pin --> HC-SR04 Echo
#define PULSE_TO_CM_DIVIDER 58 // from HC-SR04 User's Manual v1.0

void setup()
{
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  digitalWrite(TRIGGER, HIGH); //Signal abschalten

  Serial.begin(57600);
}

void loop()
{
  int distance = getDistanceInCentimeter();

  Serial.write("distance:");
  Serial.print(distance, DEC);
  Serial.write(" cm\n");

  delay(1000);
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
