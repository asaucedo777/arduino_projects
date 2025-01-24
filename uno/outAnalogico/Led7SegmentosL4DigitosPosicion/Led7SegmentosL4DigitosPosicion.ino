// Valor a mostrar
int value = 8;
// Pines de los segmentos a-g
const int segmentPins[8] = {2,3,4,5,6,7,8,13};
const int digitPins[4] = {9,10,11,12};

// Mapeo de números a segmentos
const byte numbers[10][8] = {
  //a, b, c, d, e, f, g, DP    
  { 0, 0, 0, 0, 0, 0, 1, 1 },    // 0
  { 1, 0, 0, 1, 1, 1, 1, 1 },    // 1
  { 0, 0, 1, 0, 0, 1, 0, 1 },    // 2
  { 0, 0, 0, 0, 1, 1, 0, 1 },    // 3
  { 1, 0, 0, 1, 1, 0, 0, 1 },    // 4
  { 0, 1, 0, 0, 1, 0, 0, 1 },    // 5
  { 0, 1, 0, 0, 0, 0, 0, 1 },    // 6
  { 0, 0, 0, 1, 1, 1, 1, 1 },    // 7
  { 0, 0, 0, 0, 0, 0, 0, 1 },    // 8
  { 0, 0, 0, 0, 1, 0, 0, 1 }     // 9
};

void setup() {
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  // Encendemos pines de activación de dígitos
  analogWrite(A0, 255);
  analogWrite(A1, 255);
  analogWrite(A2, 255);
  analogWrite(A3, 255);
  // Establecemos pines del dígito y los apagamos
  for (int i = 0; i < 9; i++) {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], HIGH);
  }
  Serial.begin(9600);
  Serial.println("Introduce un número del 0 al 9 y pulsa <Enter>");
}

void displayNumber(int pos, int num) {
  // Encender pines que correspondan al número
  if (num >= 0 && num <= 9) {
    for (int i = 0; i < 9; i++) {
      digitalWrite(segmentPins[i], numbers[num][i]);
    }
  } else {
    Serial.print("Número no válido ");
    Serial.println(num);
  }
  // Encendemos pines de activación de dígitos
  analogWrite(pos, 255);
  //Serial.println(pos);
}

void displayDigitos(int numero) {
  int digit_0 = numero%10;
  int digit_1 = (numero%100) / 10;
  int digit_2 = (numero%1000) / 100;
  int digit_3 = numero / 1000;
  displayNumber(0, digit_0);
  displayNumber(1, digit_1);
  displayNumber(2, digit_2);
  displayNumber(3, digit_3);
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
    value = Serial.parseInt();
    Serial.print("Value: ");
    Serial.println(value);
    if (value < 10) {
      displayNumber(0, value);
    } else {
      displayDigitos(value);
    }
    delay(2000);
    Serial.print("Número mostrado: ");
    Serial.println(value);
  } else {
    if (value < 10) {
      displayNumber(0, value);
    } else {
      displayDigitos(value);
    }
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
