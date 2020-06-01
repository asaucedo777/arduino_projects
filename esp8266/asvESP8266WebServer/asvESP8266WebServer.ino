#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <time.h>

#ifndef STASSID
#define STASSID "vodafoneBA2094"
#define STAPSK "SKJGNQGNE5Q66LTM"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;
const char *HOST_NAME = "esp8266";
const char *SERVER_NTP_1 = "pool.ntp.org";
const char *SERVER_NTP_2 = "time.nist.gov";

const int BAUD_RATE = 115200;
const int DELAY_CONNECT = 100;
const int ONE_SECOND = 1000;
const int MAX_DURATION_SECONDS = 300;
const int AMOUNT_OF_PINS = 9;

ESP8266WebServer server(80);
boolean ledBuiltinStatus = true;
int connectionTime = 0;
int scheduledMode = 0;

typedef struct record_type
{
  int pin;
  char *description;
  boolean status;
  uint8_t on_value;
  uint8_t off_value;
  time_t start0;
  time_t end0;
  time_t start1;
  time_t end1;
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
  server.handleClient();
  MDNS.update();

  // Control de programación
  if (scheduledMode == 1)
  {
    time_t ahora = time(nullptr) - 1590969600;
    for (int i = 0; i < AMOUNT_OF_PINS; i++)
    {
      // Si el pin está activado
      if (pins_array[i].status)
      {
        // Compruebo si hay que desactivar
        if (ahora < pins_array[i].start0 || ahora > pins_array[i].end0)
        {
          Serial.println("Desactivando pin digital: D" + String(i) + "-" + String(pins_array[i].pin));
          Serial.println("Ahora: " + String(ahora));
          digitalWrite(pins_array[i].pin, pins_array[i].off_value);
          pins_array[i].status = false;
        }
      }

      // Si el pin está desactivado
      if (!pins_array[i].status)
      {
        // Compruebo si hay que activar
        if (ahora > pins_array[i].start0)
        {
          if (ahora < pins_array[i].end0)
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
  pins_array[0] = (record_type){16, "\"Riego fase 1          \"", false, LOW, HIGH,
                                25200L, 25320L, 0L, 0L, "\"Estado inicial\""};
  pins_array[1] = (record_type){5, "\"Riego fase 2          \"", false, LOW, HIGH,
                                25500L, 25620L, 0L, 0L, "\"Estado inicial\""};
  pins_array[2] = (record_type){4, "\"Depuradora            \"", false, LOW, HIGH,
                                0L, 0L, 0L, 0L, "\"Estado inicial\""};
  pins_array[3] = (record_type){0, "\"Luz caseta            \"", false, LOW, HIGH,
                                0L, 0L, 0L, 0L, "\"Estado inicial\""};
  pins_array[4] = (record_type){2, "\"No utilizar (LED)     \"", false, HIGH, LOW,
                                0L, 0L, 0L, 0L, "\"Estado inicial\""};
  pins_array[5] = (record_type){14, "\"Puerta garaje interior\"", false, HIGH, LOW,
                                0L, 0L, 0L, 0L, "\"Estado inicial\""};
  pins_array[6] = (record_type){12, "\"Puerta garaje exterior\"", false, HIGH, LOW,
                                0L, 0L, 0L, 0L, "\"Estado inicial\""};
  pins_array[7] = (record_type){13, "\"Puerta casa exterior  \"", false, HIGH, LOW,
                                0L, 0L, 0L, 0L, "\"Estado inicial\""};
  pins_array[8] = (record_type){15, "\"Libre                 \"", false, HIGH, LOW,
                                0L, 0L, 0L, 0L, "\"Estado inicial\""};

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
    connectionTime % 500 == 0 ? Serial.print(" ") : Serial.print(".");
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
  server.on("/TEST", handleTest);
  server.on("/TIME", handleTime);

  server.on("/LED_BUILTIN", handleLedBuiltinGet);
  server.on("/LED_BUILTIN/SWITCH", handleLedBuiltinSwitch);
  server.on("/LED_BUILTIN/ON", handleLedBuiltinOn);
  server.on("/LED_BUILTIN/OFF", handleLedBuiltinOff);

  server.on("/DIGITAL_PINS", handleDigitalPins);
  server.on("/DIGITAL_PIN_GET", handleDigitalPinGet);
  server.on("/DIGITAL_PIN/STATUS", handleDigitalPinStatus);
  server.on("/DIGITAL_PIN/SWITCH", handleDigitalPinSwitch);
  server.on("/DIGITAL_PIN/ON", handleDigitalPinOn);
  server.on("/DIGITAL_PIN/OFF", handleDigitalPinOff);
  server.on("/DIGITAL_PIN_POST", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(204);
  });
  server.on("/DIGITAL_PIN_POST", HTTP_POST, handleDigitalPinPost);

  server.on("/SCHEDULED", handleScheduledGet);
  server.on("/SCHEDULED/SWITCH", handleScheduledSwitch);

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void initTime()
{
  configTime(1 * 3600, 0, SERVER_NTP_1, SERVER_NTP_2);
  Serial.println("Waiting for time..");
  while (!time(nullptr))
  {
    Serial.print(".");
    delay(ONE_SECOND);
  }
  Serial.println("Time configured.");
}

void handleTest()
{
  String response = "{\"test\":\"Welcome to esp8266!!\"}";
  server.send(200, "application/json", response);
}

void handleTime()
{
  time_t now = time(nullptr);
  char buff[20];
  strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
  String response = "{\"time\":" + String(now) + String("}");
  server.send(200, "application/json", response);
}

void handleLedBuiltinGet()
{
  String status = ledBuiltinStatus ? "1" : "0";
  String response = "{\"ledBuiltin\":\"" + status + String("\"}");
  server.send(200, "application/json", response);
}

void handleLedBuiltinSwitch()
{
  String response = "";
  if (!ledBuiltinStatus)
  {
    Serial.println("Encenciendo LED");
    digitalWrite(LED_BUILTIN, HIGH);
    ledBuiltinStatus = true;
    response = "{\"result\":\"LED_BUILTIN SWITCH A encendido\"}";
  }
  else
  {
    Serial.println("Apagando LED");
    digitalWrite(LED_BUILTIN, LOW);
    ledBuiltinStatus = false;
    response = "{\"result\":\"LED_BUILTIN SWITCH A apagado\"}";
  }
  server.send(200, "application/json", response);
}

void handleLedBuiltinOn()
{
  Serial.println("Encenciendo LED");
  String response = "";
  if (!ledBuiltinStatus)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    ledBuiltinStatus = true;
    response = "{\"result\":\"LED_BUILTIN SWITCH A encendido\"}";
  }
  else
  {
    response = "{\"result\":\"LED_BUILTIN SWITCH ya estaba encendido\"}";
    Serial.println("LED ya está encendido.");
  }
  server.send(200, "application/json", response);
}

void handleLedBuiltinOff()
{
  Serial.println("Apagando LED.");
  String response = "";
  if (!ledBuiltinStatus)
  {
    Serial.println("LED ya está apagado.");
    response = "{\"result\":\"LED_BUILTIN SWITCH ya estaba apagado\"}";
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
    ledBuiltinStatus = false;
    response = "{\"result\":\"LED_BUILTIN SWITCH A apagado\"}";
  }
  server.send(200, "application/json", response);
}

void handleDigitalPins()
{
  String response = "{\"pines\":[";
  for (int i = 0; i < AMOUNT_OF_PINS; i++)
  {
    response += "{\"pin\":" + String(i) +
                ", \"description\":" + String(pins_array[i].description) +
                ", \"start0\":" + String(pins_array[i].start0) +
                ", \"end0\":" + String(pins_array[i].end0) +
                ", \"status\":" + pins_array[i].status +
                ", \"result\":" + String(pins_array[i].result) +
                "}";
    if (i != 8)
    {
      response += ",";
    }
  }
  response += "]}";
  Serial.println(response);
  server.send(200, "application/json", response);
}

void handleDigitalPinGet()
{
  String response = "";
  int pin = server.arg(String("pin")).toInt();
  if (pin >= 0 && pin < AMOUNT_OF_PINS)
  {
    response += "{\"pin\":" + String(pin) +
                ", \"description\":" + String(pins_array[pin].description) +
                ", \"start0\":" + String(pins_array[pin].start0) +
                ", \"end0\":" + String(pins_array[pin].end0) +
                ", \"status\":" + pins_array[pin].status +
                ", \"result\":" + String(pins_array[pin].result) +
                "}";
  }
  else
  {
    response = "{\"error\":\"Parametro pin es obligatorio. Valor entre 0 y 8\"}";
  }
  Serial.println(response);
  server.send(200, "application/json", response);
}

void handleDigitalPinPost()
{
  String response = "";

  if (server.hasArg(String("start0")))
  {
    long pin = server.arg(String("pin")).toInt();
    long start0 = server.arg(String("start0")).toInt();
    long end0 = server.arg(String("end0")).toInt();
    long duration0 = end0 - start0;
    if (pin >= 0 && pin < AMOUNT_OF_PINS)
    {
      if (duration0 > 0L)
      {
        if (duration0 < MAX_DURATION_SECONDS)
        {
          pins_array[pin].start0 = start0;
          pins_array[pin].end0 = end0;
          pins_array[pin].result = "\"Programado fase 1\"";
          response += "{\"pin\":" + String(pin) +
                      ", \"description\":" + String(pins_array[pin].description) +
                      ", \"start0\":" + String(pins_array[pin].start0) +
                      ", \"end0\":" + String(pins_array[pin].end0) +
                      ", \"status\":" + pins_array[pin].status +
                      ", \"result\":" + String(pins_array[pin].result) +
                      "}";
        }
        else
        {
          response = "{\"error\":\"Pin digital D" + String(pin) +
                     " mal programado. Duracion maxima " + String(MAX_DURATION_SECONDS) + "\"}";
        }
      }
      else
      {
        response = "{\"error\":\"Pin digital D" + String(pin) +
                   " mal programado. Sin duración: " + String(start0) + "-" + String(end0) + "\"}";
      }
    }
    else
    {
      response = "{\"error\":\"Pin digital debe estar entre 0 y 8\"}";
    }
  }
  else
  {
    response = "{\"error\":\"Faltan parametros\"}";
  }
  Serial.println(response);
  server.send(200, "application/json", response);
}

void handleDigitalPinStatus()
{
  int pin = server.arg(String("pin")).toInt();
  if (pin >= 0 && pin < AMOUNT_OF_PINS)
  {
    digitalPinStatus(pin);
  }
  else
  {
    Serial.println("No se ha recuperado el parametro");
  }
}

void handleDigitalPinSwitch()
{
  int pin = server.arg(String("pin")).toInt();
  if (pin >= 0 && pin < AMOUNT_OF_PINS)
  {
    digitalPinSwitch(pin);
  }
  else
  {
    Serial.println("No se ha recuperado el parametro");
  }
}

void handleDigitalPinOn()
{
  int pin = server.arg(String("pin")).toInt();
  if (pin >= 0 && pin < AMOUNT_OF_PINS)
  {
    digitalPinOn(pin);
  }
  else
  {
    Serial.println("No se ha recuperado el pin a activar");
  }
}

void handleDigitalPinOff()
{
  int pin = server.arg(String("pin")).toInt();
  if (pin >= 0 && pin < AMOUNT_OF_PINS)
  {
    digitalPinOff(pin);
  }
  else
  {
    Serial.println("No se ha recuperado el pin a desactivar");
  }
}

void handleScheduledGet()
{
  String response = "{\"scheduledMode\":" + String(scheduledMode) + "}";
  server.send(200, "application/json", response);
}

void handleScheduledSwitch()
{
  String response = "";
  if (scheduledMode == 1)
  {
    response = "{\"scheduledMode\":0}";
  }
  else
  {
    response = "{\"scheduledMode\":1}";
  }
  scheduledMode = scheduledMode == 1 ? 0 : 1;
  server.send(200, "application/json", response);
}

void handleNotFound()
{
  String response = "";
  String message = "\"File Not Found\",\n";
  message += "\"URI\":\"";
  message += server.uri();
  message += "\"\n\"Method\":\"";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\"\nArguments\":\"";
  message += server.args();
  message += "\"";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  response = "{\"error\":" + message + "\"}";
  server.send(404, "application/json", response);
}

void digitalPinStatus(int pin)
{
  String response = "";
  response += "{\"status\":" + String(pins_array[pin].status) + "}";
  server.send(200, "application/json", response);
}

void digitalPinSwitch(int pin)
{
  String response = "";
  if (pins_array[pin].status)
  {
    Serial.println("Desactivando pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin));
    digitalWrite(pins_array[pin].pin, pins_array[pin].off_value);
    pins_array[pin].status = false;
    pins_array[pin].result = "\"Desactivado pin digital\"";
  }
  else
  {
    Serial.println("Activando pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin));
    digitalWrite(pins_array[pin].pin, pins_array[pin].on_value);
    pins_array[pin].status = true;
    pins_array[pin].result = "\"Activado pin digital\"";
  }
  response += "{\"pin\":" + String(pin) +
              ", \"description\":" + String(pins_array[pin].description) +
              ", \"start0\":" + String(pins_array[pin].start0) +
              ", \"end0\":" + String(pins_array[pin].end0) +
              ", \"status\":" + pins_array[pin].status +
              ", \"result\":" + String(pins_array[pin].result) +
              "}";
  server.send(200, "application/json", response);
}

void digitalPinOn(int pin)
{
  String response = "";
  if (pins_array[pin].status)
  {
    Serial.println("Pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin) + " ya estaba activado.");
  }
  else
  {
    Serial.println("Activando pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin));
    digitalWrite(pins_array[pin].pin, pins_array[pin].on_value);
    pins_array[pin].status = true;
    pins_array[pin].start0 = time(nullptr);
    pins_array[pin].end0 = pins_array[pin].start0 + 300L;
    pins_array[pin].start1 = 0L;
    pins_array[pin].end1 = 0L;
    pins_array[pin].result = "\"Activado pin digital\"";
  }
  response += "{\"pin\":" + String(pin) +
              ", \"description\":" + String(pins_array[pin].description) +
              ", \"start0\":" + String(pins_array[pin].start0) +
              ", \"end0\":" + String(pins_array[pin].end0) +
              ", \"status\":" + pins_array[pin].status +
              ", \"result\":" + String(pins_array[pin].result) +
              "}";
  server.send(200, "application/json", response);
}

void digitalPinOff(int pin)
{
  String response = "";
  if (pins_array[pin].status)
  {
    Serial.println("Desactivando pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin));
    digitalWrite(pins_array[pin].pin, pins_array[pin].off_value);
    pins_array[pin].status = false;
    pins_array[pin].start0 = 0L;
    pins_array[pin].end0 = 0L;
    pins_array[pin].start1 = 0L;
    pins_array[pin].end1 = 0L;
    pins_array[pin].result = "\"Desactivado pin digital\"";
  }
  else
  {
    Serial.println("Pin digital: D" + String(pin) + "-" +
                   String(pins_array[pin].pin) + " ya estaba desactivado.");
  }
  response += "{\"pin\":" + String(pin) +
              ", \"description\":" + String(pins_array[pin].description) +
              ", \"start0\":" + String(pins_array[pin].start0) +
              ", \"end0\":" + String(pins_array[pin].end0) +
              ", \"status\":" + pins_array[pin].status +
              ", \"result\":" + String(pins_array[pin].result) +
              "}";
  server.send(200, "application/json", response);
}
