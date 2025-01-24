int led_a = 7;
int led_b = 8;
int led_c = 9;
int led_d = 10;
int led_e = 11;
int led_f = 12;
int led_g = 13;
int brightness = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(led_a, OUTPUT);
  pinMode(led_b, OUTPUT);
  pinMode(led_c, OUTPUT);
  pinMode(led_d, OUTPUT);
  pinMode(led_e, OUTPUT);
  pinMode(led_f, OUTPUT);
  pinMode(led_g, OUTPUT);
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
    analogWrite(led_a, brightness);
    analogWrite(led_b, brightness);
    analogWrite(led_c, brightness);
    analogWrite(led_d, brightness);
    analogWrite(led_e, brightness);
    analogWrite(led_f, brightness);
    analogWrite(led_g, brightness);
  }
}

/*
Experimento:
Conecto una resistencia de 150 Ohmios en cada uno de los pines del LED (7 A 13)
  -> VCC -> 5 voltios
  -> D7 A D13 -> a..g
Cargo el programa en Arduino UNO
  - Al ejecutarse se ecienden todos los segmentos
*/
