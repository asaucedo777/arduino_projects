/*
  ReadAnalogVoltage

  Reads an analog input on pin 0, converts it to voltage, and prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/ReadAnalogVoltage
*/

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

static bool measure_available(double *value) {
  static unsigned long ts_start = millis();
  if (millis() - ts_start > 500ul) {
    // read the input on analog pin 0:
    int sensorValue = analogRead(A0);
    // Serial.print(String("") + sensorValue + String(","));
    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    *value = sensorValue * (5.0 / 1023.0);
    ts_start = millis();
    return (true);
  }
  return (false);
}

// the loop routine runs over and over again forever:
void loop() {
  double voltage;
  // execute every second
  if (measure_available(&voltage)){
    // print out the value you read:
    Serial.println(String("") + voltage + String("v,"));
  }
}

/*
Experimento:
  Conecto un potenciómetro en la "protoboard":
    - GND
    - + 5v
    - A0
  Conecto el test entre GND y A0

  Con el potenciómetro al máximo:
    - El tester da una medida de 5.2 voltios
    - El monitor serial me da una medida de 5 voltios
  Con el potenciómetro al mínimo:
    - El tester da una medida de 0.009 voltios
    - El monitor serial me da una medida de 0 voltios
 Por debajo de 0.8 voltios el test no es capaz de medir

Prueba de potenciómetros:
  * Potenciómetro marcado como ? (no se sabe si funciona)
    - Si giro totalmente en sentido horario -> 0.00 v
    - Si giro totalmente en sentido antohorario -> 5.00 v
  TODOS FUNCIONAN!!
*/
