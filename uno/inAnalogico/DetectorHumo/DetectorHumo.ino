/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogReadSerial
*/

const int pinHayHumo = 6;
int val = 0;
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(pinHayHumo, INPUT);
  Serial.println("Echa humo en el sensor...");
}

// the loop routine runs over and over again forever:
void loop() {
  // read digital input 6
  val = digitalRead(pinHayHumo);
  if (val != 0) {
    Serial.println("Cuidado hay humo!");
    Serial.println(val);
    delay(1000);
  }
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // print out the value you read:
  if (sensorValue < 1000) {
    Serial.println(sensorValue);
    delay(2000);  // delay in between reads for stability
  }
}

/*
Experimento:
0. El valor en reposo del sensor es aprox 89
1. Acerco el humo de un papel quemado y el sensor se acerca a 500. El sensor digital toma el valor 1 y se mantiene (sigue oliendo a humo)
*/
