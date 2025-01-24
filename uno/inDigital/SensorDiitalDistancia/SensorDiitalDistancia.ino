#include <DistanceSensor.h>

// Pins
// Replace as needed
constexpr int TrigPin = 2;
constexpr int EchoPin = 3;

// The DistanceSensor object
DistanceSensor<TrigPin, EchoPin> sensor;

void setup()
{
  // Initialize the object
  // If you don't want to use the default
  // pinmodes of OUTPUT and INPUT for trig and echo,
  // don't use this and setup pins yourself
  sensor.begin();

  // For the example
  Serial.begin(9600);
}

void loop()
{
  // Query the sensor
  // Ensure this gets called as fast as possible
  // Note that it will only return meaningful
  // results after the sensor has completed
  // the measurement, so it is not a call-once
  // solution
  int result = sensor.tick();

  // tick() retunrs NREADY if not ready, or
  // ERR on error (like timeout)
  if (result == sensor.NREADY) return;

  if (result == sensor.ERR)
  {
    Serial.println("error occured");
    return;
  }

  // `result` contains the distance in centimeters
  Serial.print("Distance: ");
  Serial.print(result);
  Serial.println("cm");

  // Wait a bit
  delay(200);
}