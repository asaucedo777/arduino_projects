int led = 9;
int brightness = 0;
int increment = 10;

void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
  Serial.begin(9600);
}

static bool available() {
  static unsigned long measurement_timestamp = millis();
  /* Measure once every four seconds. */
  if (millis() - measurement_timestamp > 300ul) {
    // initialize timer
    measurement_timestamp = millis();
    return (true);
  }
  return (false);
}

void loop() {
  if (available()) {
    // put your main code here, to run repeatedly:
    analogWrite(led, brightness);
    // Print serial
    Serial.println(String("Brillo: ") + String(brightness));
    // change the brightness
    brightness += increment;
    // reverse direction
    if (brightness <= 0 || brightness >= 255) {
      increment = -increment;
    }
  }
}

/*
Experimento:
Conecto una resistencia de aprox 220 Ohm y un Led en la placa de pruebas.
  -> GND - resistencia -> - LED
  -> D9 -> + LED
Cargo el programa en Arduino UNO
Al ejecutarse se aprecia como va aumentando el brillo del led y luego disminuyendo hasta apagarse
*/
