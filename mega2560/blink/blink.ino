#include "Arduino.h"

const int PORT_LED = LED_BUILTIN;

void setup()
{
  pinMode(PORT_LED, OUTPUT);
}

void loop()
{
  digitalWrite(PORT_LED, HIGH);
  delay(200);
  digitalWrite(PORT_LED, LOW);
  delay(200);
}