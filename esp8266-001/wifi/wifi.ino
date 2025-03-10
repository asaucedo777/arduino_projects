/*
  esp8266-001 test wifi
*/
#include "DHT_Async.h"
#include "ESP8266WiFi.h"

DHT_Async dht_sensor(1, DHT_TYPE_11);
const String MY_SSID = "MiRedWifi";
const String MY_PASSWORD = "MiPassword";
const String server = "192.168.4.1";
const int httpPort = 80;
const unsigned long measure_interval = 5000; // Define measure_interval in milliseconds

static bool measure(float *temperature, float *humidity, unsigned long interval)
{
  static unsigned long measurement_timestamp = millis();
  /* Measure once every four seconds. */
  if (millis() - measurement_timestamp > interval)
  {
    if (dht_sensor.measure(temperature, humidity))
    {
      measurement_timestamp = millis();
      return (true);
    }
  }
  return (false);
}

static void send_to_server(String server, uint16_t server_port, float temperature, float humidity)
{
  WiFiClient client;
  if (!client.connect(server, server_port))
  {
    return;
  }
  client.print(String("GET /temperature?value=") + String(temperature) 
    + " HTTP/1.1\r\n" + "Host: " + server + "\r\n" + "Connection: close\r\n\r\n");
  client.print(String("GET /humidity?value=") + String(humidity) 
    + " HTTP/1.1\r\n" + "Host: " + server + "\r\n" + "Connection: close\r\n\r\n");
}

void setup()
{
  WiFi.begin(MY_SSID, MY_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(2000);
  }
}

void loop()
{
  float temperature;
  float humidity;
  send_to_server(server, httpPort, 10.00, humidity);
  if (measure(&temperature, &humidity, measure_interval))
  {
    send_to_server(server, httpPort, temperature, humidity);
  }
}

/*
Experimento:

CARGA DEL SKETCH EN EL ESP8266 ESP-01:
  * primero preparar el adaptador USB a TTL CH340G para programar el ESP8266
    - Soldar pulsador entre los pines VCC y GP0 (PINES 5 Y 7)
  * con el pulsador presionado conectar el adaptador USB a TTL CH340G al PC
    - Cargar el sketch blink en el ESP8266

COMPONENTES DE RED WIFI:
  * Configurar un punto de acceso WIFI con el SSID y PASSWORD definidos en el sketch
  * Conectar un servidor al punto de acceso con server = "192.168.4.1"

MONTAJE DE COMPONENTES:
  * alimentar con 3.3v el ESP-01
  * conectar SENSOR DHT11 al pin 2 del ESP-01

*/