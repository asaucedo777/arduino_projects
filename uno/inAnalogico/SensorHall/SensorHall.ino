/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogReadSerial
*/

const int pinHayIman = 6;
int val = 0;
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(pinHayIman, INPUT);
  Serial.println("Acerca un imán al sensor...");
}

// the loop routine runs over and over again forever:
void loop() {
  // read digital input 6
  val = digitalRead(pinHayIman);
  if (val != 0) {
    Serial.println("Imán detectado!");
    Serial.println(val);
    delay(1000);
  }
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // print out the value you read:
  if (sensorValue > 40) {
    Serial.println(sensorValue);
    delay(2000);  // delay in between reads for stability
  }
}

// Experimento:
/*
0. El Sensor en reposo arroja una lectura de 504
1. Acerco un imán férrico al sensor obteniendo los siguienes valores de lectura:
  polo positivo
  - 3 cm -> 510
  - 2 cm -> 520
  - 1 cm -> 565
  - 0.5 cm -> 634
  - 0.1 cm -> 760
  - 0.0 cm -> 831
  polo negativo
  - 3 cm -> 501
  - 2 cm -> 495
  - 1 cm -> 460
  - 0.5 cm -> 432
  - 0.1 cm -> 425
  - 0.0 cm -> 423

*/
