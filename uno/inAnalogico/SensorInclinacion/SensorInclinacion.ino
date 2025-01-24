/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogReadSerial
*/

const int pinInclinacion = 6;
int val = 0;
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(pinInclinacion, INPUT);
  Serial.println("Inclina el sensor en varias direcciones...");
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // print out the value you read:
  Serial.print("" + String(sensorValue) + ",");
  delay(1000);  // delay in between reads for stability
}

/*
0. Valor del sensor en reposo: aprox 0
1. Cuando toco el sensor con el dedo o con un material conductor, el valor del puerto análogico es 1023
*/