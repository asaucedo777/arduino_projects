#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_PCF8574 lcd(0x27);
int show = -1;
byte dotOff[] = { 0b00000, 0b01110, 0b10001, 0b10001,0b10001, 0b01110, 0b00000, 0b00000 };
byte dotOn[] = { 0b00000, 0b01110, 0b11111, 0b11111,0b11111, 0b01110, 0b00000, 0b00000 };

void setup() {
  int error;
  Serial.begin(115200);
  Serial.println("LCD...");
  Serial.println("Probing for PCF8574 on address 0x27...");
  // See http://playground.arduino.cc/Main/I2cScanner how to test for a I2C device.
  Wire.begin();
  Wire.beginTransmission(0x27);
  error = Wire.endTransmission();
  Serial.print("Error: ");
  Serial.print(error);
  delay(5000);
  if (error == 0) {
    Serial.println(": LCD found.");
    show = 0;
    lcd.begin(16, 2);  // initialize the lcd
    lcd.createChar(1, dotOff);
    lcd.createChar(2, dotOn);
  } else {
    Serial.println(": LCD not found.");
  }
}

void loop() {
  lcd.setBacklight(255);
  lcd.home();
  //lcd.clear();
  // 1. Imprimir text y desplazarlo hacia la izquierda para poderlo leer
  // lcd.print("Este es el texto más largo ");
  // for (int i=0; i < 16; i++) {
  //   lcd.scrollDisplayLeft();
  //   delay(200);
  // }
  // 2. Imprimir temporizador de segundos, cada 200 ms en la segunda línea
  // lcd.setCursor(0,1);
  // lcd.print(String(millis()/1000) + String(" segundos"));
  // 3. Imprimir caracter en una posición de la pantalla y desplazarlo por la pantalla
  lcd.setCursor(7,0);
  lcd.print("A");
  int i = 7;
  while(true) {
    lcd.setCursor(i,0);
    lcd.print(" ");
    i++;
    lcd.setCursor(i,0);
    lcd.print("A");
    if (i==16) {
      i = 0;
    }
    delay(200);
  }
}  // loop()

/* 

*/
