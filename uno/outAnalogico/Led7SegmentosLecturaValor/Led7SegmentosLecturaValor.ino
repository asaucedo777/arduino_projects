// Pines de los segmentos a-g
const int segmentPins[7] = {7,8,9,10,11,12,13};

// Mapeo de números a segmentos
const byte numbers[10][7] = {
  //  a,   b,   c,   d,   e,   f,   g
  {LOW,  LOW,  LOW,  LOW,  LOW,  HIGH,  LOW},  // 0
  {HIGH, LOW,  LOW,  HIGH, HIGH, HIGH, HIGH},  // 1
  {LOW,  LOW,  HIGH, LOW,  LOW,  LOW,  HIGH},   // 2
  {LOW,  LOW,  LOW,  LOW,  HIGH, LOW, HIGH},   // 3
  {HIGH, LOW,  LOW,  HIGH, HIGH, LOW,  LOW},   // 4
  {LOW,  HIGH, LOW,  LOW,  HIGH, LOW,  LOW},   // 5
  {LOW,  HIGH, LOW,  LOW,  LOW,  LOW,  LOW},   // 6
  {LOW,  LOW,  LOW,  HIGH, HIGH, HIGH, HIGH},  // 7
  {LOW,  LOW,  LOW,  LOW,  LOW,  LOW,  LOW},   // 8
  {LOW,  LOW,  LOW,  LOW,  HIGH, LOW,  LOW}    // 9
};

void displayNumber(int num) {
  byte segments = numbers[num];  if (num >= 0 && num <= 9) {
    for (int pin = 7; pin < 14; pin++) {
      digitalWrite(pin, numbers[num][pin - 7]);
      // digitalWrite(pin, HIGH);
    }
  } else {
    Serial.print("Número no válido ");
    Serial.println(num);
  }
}

void setup() {
  for (int pin = 7; pin < 14; pin++) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }
  Serial.begin(9600);
  Serial.println("Introduce un número del 0 al 9 y pulsa <Enter>");
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
  if (Serial.available()) {
    int value = Serial.parseInt();
    Serial.print("Value: ");
    Serial.println(value);
    displayNumber(value);
    Serial.print("Número mostrado: ");
    Serial.println(value);
  }
  //   // Limpiar el buffer serial
  // while (Serial.available()) {
  //   Serial.read();
  // }
}

/*
Experimento:
Conecto una resistencia de 150 Ohmios en cada uno de los pines del LED (7 A 13)
  -> VCC -> 5 voltios
  -> D7 A D13 -> a..g
Cargo el programa en Arduino UNO
  - Al ejecutarse se ecienden todos los segmentos
*/
