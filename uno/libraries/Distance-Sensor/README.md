# Non-blocking HC-SR04 and compatible Arduino lib
  This is a simple-to-use library that doesn't block
the flow of execution, for use with HC-SR04's and the
like.
  Non-blocking necessarily brings along the cumbersome
state-machine, so while developing a project, I decided
to upload this part as a simple library.

## Examples

```cpp
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
```

## Troubleshooting
- Check wiring and pin connections
- Try detecting another surface (for example, a wall)
- Try moving the object closer (but not too close)

As some have mentioned, the sensor has a hard time detecting "soft" surfaces,
try a harder one like a wall or book

## Credits
- Thanks to Nicholas_N for the great guide on the HY-SRF05 [link](https://projecthub.arduino.cc/Nicholas_N/distance-measurement-with-an-ultrasonic-sensor-hy-srf05-bf2923)

## License
This library is fully released into the public domain. Do whatever. See LICENSE
