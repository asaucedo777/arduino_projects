#include "VirtualWire.h"

void setup()
{
  Serial.begin(9600);
  vw_setup(2000);   // Velocidad de datos en bits por segundo
  vw_set_rx_pin(3); // Pin de recepción
  vw_rx_start();    // Iniciar el receptor
}

void loop()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen))
  { // Mensaje recibido
    Serial.print("Mensaje recibido: ");
    for (int i = 0; i < buflen; i++)
    {
      Serial.print((char)buf[i]);
    }
    Serial.println();
  }
}
