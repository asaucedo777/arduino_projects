/*
  ESP8266 Blink by Simon Peter
  Blink the blue LED on the ESP-01 module
  This example code is in the public domain

  The blue LED on the ESP-01 module is connected to GPIO1
  (which is also the TXD pin; so we cannot use Serial.print() at the same time)

  Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
*/

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output
}

// the loop function runs over and over again forever
void loop()
{
  digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (Note that LOW is the voltage level
  // but actually the LED is on; this is because
  // it is active low on the ESP-01)
  delay(200);                     // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
  delay(200);                     // Wait for two seconds (to demonstrate the active low LED)
}


/* 
Experimento:

* primero preparar el adaptador USB a TTL CH340G para programar el ESP8266
  - Soldar pulsador entre los pines VCC y GP0 (PINES 5 Y 7)
* con el pulsador presionado conectar el adaptador USB a TTL CH340G al PC
  - Cargar el sketch blink en el ESP8266
* conectar el ESP8266 a un adaptador de protoboard
* alimentar con 3.3v
* conectar LED entre el pin GP2 y GND

*/