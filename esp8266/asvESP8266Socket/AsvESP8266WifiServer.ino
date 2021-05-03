#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ESP8266mDNS.h>
#include <time.h>

#ifndef STASSID
#define STASSID "MiFibra-0D4B"
#define STAPSK "WSWpdMi6"
// #define STASSID "vodafoneBA2094"
// #define STAPSK "SKJGNQGNE5Q66LTM"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;
const char *HOST_NAME = "esp8266";
const char *SERVER_NTP_1 = "pool.ntp.org";
const char *SERVER_NTP_2 = "time.nist.gov";

const int BAUD_RATE = 115200;
const int DELAY_CONNECT = 100;
const int ONE_SECOND = 1000;
const int MAX_DURATION_SECONDS = 900;
const int AMOUNT_OF_PINS = 9;
const int ONE_DAY = 24 * 60 * 60;

WiFiServer wifiServer(80);
boolean ledBuiltinStatus = true;
int connectionTime = 0;
int scheduledMode = 0;

typedef struct record_type
{
  int index;
  int pin;
  char *description;
  boolean status;
  boolean disabled;
  uint8_t on_value;
  uint8_t off_value;
  time_t start0;
  time_t start1;
  long duration0;
  long duration1;
  char *result;
} pin_type;
record_type pins_array[AMOUNT_OF_PINS];

void setup(void)
{
  initSerial();
  initPins();
  initWifi();
  initDns();
  initServer();
  initTime();
}

void loop(void)
{
  MDNS.update();

  // Control de programación
  if (scheduledMode == 1)
  {
    // TODO Quitamos fecha, nos quedamos con la hora sólo
    time_t ahora = time(nullptr) % ONE_DAY;
    for (int i = 0; i < AMOUNT_OF_PINS; i++)
    {
      // Si el pin no está deshabilitado y está activado
      if (!pins_array[i].disabled && pins_array[i].status)
      {
        // Compruebo si hay que desactivar
        if (ahora < pins_array[i].start0 || ahora > (pins_array[i].start0 + pins_array[i].duration0))
        {
          Serial.println("Desactivando pin digital: D" + String(i) + "-" + String(pins_array[i].pin));
          Serial.println("Ahora: " + String(ahora));
          digitalWrite(pins_array[i].pin, pins_array[i].off_value);
          pins_array[i].status = false;
        }
      }

      // Si el pin está desactivado
      if (!pins_array[i].disabled && !pins_array[i].status)
      {
        // Compruebo si hay que activar
        if (ahora > pins_array[i].start0)
        {
          if (ahora > pins_array[i].start0 && ahora < (pins_array[i].start0 + pins_array[i].duration0))
          {
            Serial.println("Activando pin digital: D" + String(i) + "-" + String(pins_array[i].pin));
            Serial.println("Ahora: " + String(ahora));
            digitalWrite(pins_array[i].pin, pins_array[i].on_value);
            pins_array[i].status = true;
          }
        }
      }
    }
  }
}

void initPins()
{
  pins_array[0] = (record_type){0, 16, "\"Riego fase 1          \"", false, false, LOW, HIGH, 0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, "\"Estado inicial\""};
  pins_array[1] = (record_type){1, 5,  "\"Riego fase 2          \"", false, false, LOW, HIGH, 0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, "\"Estado inicial\""};
  pins_array[3] = (record_type){2, 4,  "\"Luz caseta            \"", false, true, LOW, HIGH, 0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, "\"Estado inicial\""};
  pins_array[2] = (record_type){3, 0,  "\"Depuradora            \"", false, true, LOW, HIGH, 0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, "\"Estado inicial\""};
  pins_array[4] = (record_type){4, 2,  "\"No utilizar (LED)     \"", false, true, HIGH, LOW, 0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, "\"Estado inicial\""};
  pins_array[5] = (record_type){5, 14, "\"Puerta garaje interior\"", false, true, HIGH, LOW, 0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, "\"Estado inicial\""};
  pins_array[6] = (record_type){6, 12, "\"Puerta garaje exterior\"", false, true, HIGH, LOW, 0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, "\"Estado inicial\""};
  pins_array[7] = (record_type){7, 13, "\"Puerta casa exterior  \"", false, true, HIGH, LOW, 0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, "\"Estado inicial\""};
  pins_array[8] = (record_type){8, 15, "\"Libre                 \"", false, true, HIGH, LOW, 0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, "\"Estado inicial\""};

  for (int i = 0; i < AMOUNT_OF_PINS; i++)
  {
    pinMode(pins_array[i].pin, OUTPUT);
    pins_array[i].status ? digitalWrite(pins_array[i].pin, pins_array[i].on_value) : digitalWrite(pins_array[i].pin, pins_array[i].off_value);
  }
  Serial.println("Pines digitales inicializados.");

  pinMode(LED_BUILTIN, OUTPUT);
  ledBuiltinStatus ? digitalWrite(LED_BUILTIN, HIGH) : digitalWrite(LED_BUILTIN, LOW);
}

void initSerial()
{
  Serial.begin(BAUD_RATE);
  while (!Serial)
  {
    ;
  }
  Serial.print("Serial BAUD_RATE: ");
  Serial.println(BAUD_RATE);
}

void initWifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    connectionTime % 500 == 0 ? Serial.print("\n") : Serial.print(".");
    delay(DELAY_CONNECT);
    connectionTime += DELAY_CONNECT;
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connection time: ");
  Serial.println(connectionTime);
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

void initDns()
{
  if (MDNS.begin(HOST_NAME))
  {
    Serial.print("MDNS host name: ");
    Serial.println(HOST_NAME);
  }
}

void initServer()
{
  wifiServer.begin();
  Serial.println("HTTP wifiServer started");
  connectionTime = 0;
  while (!wifiServer.available()) {
    connectionTime % 500 == 0 ? Serial.print("\n") : Serial.print("=");
    delay(DELAY_CONNECT);
    connectionTime += DELAY_CONNECT;
    Serial.print("=");
  }
  Serial.println("HTTP wifiServer available");
}

void initTime()
{
  configTime(2 * 3600, 0, SERVER_NTP_1, SERVER_NTP_2);
  Serial.println("Waiting for time..");
  while (!time(nullptr))
  {
    Serial.print(".");
    delay(ONE_SECOND);
  }
  Serial.println("Time configured.");
  ledBuiltinStatus = true;
  ledBuiltinStatus ? digitalWrite(LED_BUILTIN, LOW) : digitalWrite(LED_BUILTIN, HIGH);
}

void handleTest()
{
  //wifiServer.send(200, "application/json", "{\"test\":\"Welcome to esp8266!!\"}");
  wifiServer.print("Test");
}

