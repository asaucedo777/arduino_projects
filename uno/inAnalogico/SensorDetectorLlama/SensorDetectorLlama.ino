/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogReadSerial
*/

const int pinHayLlama = 6;
int val = 0;
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(pinHayLlama, INPUT);
  Serial.println("Acerca una llama al sensor...");
}

// the loop routine runs over and over again forever:
void loop() {
  // read digital input 6
  val = digitalRead(pinHayLlama);
  if (val != 0) {
    Serial.println("Fuego, Fuego!");
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

/*
0. Valor del sensor en reposo: min 810, max 825 (min con regulador girado totalmente en sentido horario, max en sentido contrario) 
0. Valor sensor digital 0 por defecto (no hay llama)
1. Acerco un mechero encendido a estas distancias:
  - 30 cm -> 0
  - 20 cm -> 0
  - 7 cm -> 0 -> 500
  - 6 cm -> 1 -> 300
  - 2 cm -> 1 -> 42
2. Tapo con la mano el sensor y la medición aumenta a 1000
*/