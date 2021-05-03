#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
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
const int WIFI_CHECK_INTERVAL = 30000;

ESP8266WebServer server(80);
boolean ledBuiltinStatus = true;
int connectionTime = 0;
int scheduledMode = 0;
unsigned long currentMillis = millis();
unsigned long previousMillis = millis();
wl_status_t wifiStatus;

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
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
  initDns();
  initServer();
  initTime();
}

void loop(void)
{
  currentMillis = millis();
  if (currentMillis - previousMillis >= WIFI_CHECK_INTERVAL)
  {
    Serial.print("Checking WiFi status...");
    wifiStatus = WiFi.status();
    Serial.print("WiFi status: " + wifiStatus);
    previousMillis = currentMillis;
  }
  if (wifiStatus == WL_CONNECTED)
  {
    // Manejar peticiones HTTP
    server.handleClient();
    MDNS.update();
    delay(0);
    // Actualizar pines
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
    } else {
      // Modo programación (no actualiza pines)
    }
  } else {
    // Pérdida de conexión WiFi
    time_t now = time(nullptr);
    char buff[20];
    strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
    Serial.print("WiFi LOST. " + String(buff));
    delay(10000);
  }
}

void initPins()
{
  pins_array[0] = (record_type){0, 16, (char *)"\"Riego fase 1          \"", false, false, LOW,  HIGH, 0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, (char *)"\"Estado inicial\""};
  pins_array[1] = (record_type){1, 5,  (char *)"\"Riego fase 2          \"", false, false, LOW,  HIGH, 0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, (char *)"\"Estado inicial\""};
  pins_array[3] = (record_type){2, 4,  (char *)"\"Luz caseta            \"", false, true,  LOW,  HIGH, 0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, (char *)"\"Estado inicial\""};
  pins_array[2] = (record_type){3, 0,  (char *)"\"Depuradora            \"", false, true,  LOW,  HIGH, 0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, (char *)"\"Estado inicial\""};
  pins_array[4] = (record_type){4, 2,  (char *)"\"No utilizar (LED)     \"", false, true,  HIGH, LOW,  0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, (char *)"\"Estado inicial\""};
  pins_array[5] = (record_type){5, 14, (char *)"\"Puerta garaje interior\"", false, true,  HIGH, LOW,  0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, (char *)"\"Estado inicial\""};
  pins_array[6] = (record_type){6, 12, (char *)"\"Puerta garaje exterior\"", false, true,  HIGH, LOW,  0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, (char *)"\"Estado inicial\""};
  pins_array[7] = (record_type){7, 13, (char *)"\"Puerta casa exterior  \"", false, true,  HIGH, LOW,  0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, (char *)"\"Estado inicial\""};
  pins_array[8] = (record_type){8, 15, (char *)"\"Libre                 \"", false, true,  HIGH, LOW,  0L, 0L, MAX_DURATION_SECONDS, MAX_DURATION_SECONDS, (char *)"\"Estado inicial\""};

  for (int i = 0; i < AMOUNT_OF_PINS; i++)
  {
    pinMode(pins_array[i].pin, OUTPUT);
    pins_array[i].status
        ? digitalWrite(pins_array[i].pin, pins_array[i].on_value)
        : digitalWrite(pins_array[i].pin, pins_array[i].off_value);
  }
  Serial.println("Pines digitales inicializados.");

  pinMode(LED_BUILTIN, OUTPUT);
  ledBuiltinStatus
      ? digitalWrite(LED_BUILTIN, HIGH)
      : digitalWrite(LED_BUILTIN, LOW);
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
  wifiStatus = WL_CONNECTED;
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Reconnecting if lost.");
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
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
  server.send(200, "application/json", "{\"test\":\"Welcome to esp8266!!\"}");
}

void handleTime()
{
  time_t now = time(nullptr);
  char buff[20];
  strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
  server.send(200, "application/json", "{\"time\":" + String(now % (ONE_DAY)) + String("}"));
}

void handleLedBuiltinGet()
{
  String status = ledBuiltinStatus ? "1" : "0";
  server.send(200, "application/json", "{\"ledBuiltin\":\"" + status + String("\"}"));
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
    response += pinToJson(pins_array[i]);
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
    response = pinToJson(pins_array[pin]);
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

  if (server.hasArg(String("pin")) 
      && server.hasArg(String("start0")) 
      && server.hasArg(String("start1")) 
      && server.hasArg(String("duration0")) 
      && server.hasArg(String("duration1")))
  {
    long pin = server.arg(String("pin")).toInt();
    long start0 = server.arg(String("start0")).toInt();
    long start1 = server.arg(String("start1")).toInt();
    long duration0 = server.arg(String("duration0")).toInt();
    long duration1 = server.arg(String("duration1")).toInt();
    if (pin >= 0 && pin < AMOUNT_OF_PINS)
    {
      if (duration0 > 0L)
      {
        if (duration0 > MAX_DURATION_SECONDS 
            || duration1 > MAX_DURATION_SECONDS)
        {
          response = "{\"error\":\"Pin digital D" + String(pin) +
                     " mal programado. Duracion maxima " + String(MAX_DURATION_SECONDS) + "\"}";
        }
        else
        {
          pins_array[pin].start0 = start0;
          pins_array[pin].start1 = start1;
          pins_array[pin].duration0 = duration0;
          pins_array[pin].duration1 = duration1;
          pins_array[pin].result = (char *)"\"Programado fase 1\"";
          response = pinToJson(pins_array[pin]);
        }
      }
      else
      {
        if (duration0 == 0L)
        {
          pins_array[pin].start0 = start0;
          pins_array[pin].start1 = start1;
          pins_array[pin].duration0 = duration0;
          pins_array[pin].duration1 = duration1;
          pins_array[pin].result = (char *)"\"Desprogramado fase 1\"";
          response = pinToJson(pins_array[pin]);
        }
        else
        {
          response = "{\"error\":\"Pin digital D" + String(pin) +
                     " mal programado. Duracion negativa: " + String(start0) + "-" + String(start1) + "\"}";
        }
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
  server.send(200, "application/json", "{\"scheduledMode\":" + String(scheduledMode) + "}");
}

void handleScheduledSwitch()
{
  String response = scheduledMode == 1 ? "{\"scheduledMode\":0}" : "{\"scheduledMode\":1}";
  scheduledMode = scheduledMode == 1 ? 0 : 1;
  server.send(200, "application/json", response);
}

void handleNotFound()
{
  String message = "\"Resource Not Found\",";
  message += "\"URI\":\"";
  message += server.uri();
  message += "\", \"Method\":\"";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\", \"Arguments\":\"";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += "" + server.argName(i) + ": " + server.arg(i) + ", ";
  }
  server.send(404, "application/json", "{\"error\":" + message + "\"}");
}

void digitalPinStatus(int pin)
{
  server.send(200, "application/json", "{\"status\":" + String(pins_array[pin].status) + "}");
}

void digitalPinSwitch(int pin)
{
  String response = "";
  if (pins_array[pin].disabled)
  {
    response = "{\"error\":\"Pin desactivado\"}";
  }
  else
  {
    if (pins_array[pin].status)
    {
      Serial.println("Desactivando pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin));
      digitalWrite(pins_array[pin].pin, pins_array[pin].off_value);
      pins_array[pin].status = false;
      pins_array[pin].result = (char *)"\"Desactivado pin digital\"";
    }
    else
    {
      Serial.println("Activando pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin));
      digitalWrite(pins_array[pin].pin, pins_array[pin].on_value);
      pins_array[pin].status = true;
      pins_array[pin].result = (char *)"\"Activado pin digital\"";
    }
    response = pinToJson(pins_array[pin]);
  }
  server.send(200, "application/json", response);
}

void digitalPinOn(int pin)
{
  String response = "";
  if (pins_array[pin].disabled)
  {
    response = "{\"error\":\"Pin desactivado\"}";
  }
  else
  {
    if (pins_array[pin].status)
    {
      Serial.println("Pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin) + " ya estaba activado.");
    }
    else
    {
      Serial.println("Activando pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin));
      digitalWrite(pins_array[pin].pin, pins_array[pin].on_value);
      pins_array[pin].status = true;
      pins_array[pin].result = (char *)"\"Activado pin digital\"";
    }
    response = pinToJson(pins_array[pin]);
  }
  server.send(200, "application/json", response);
}

void digitalPinOff(int pin)
{
  String response = "";
  if (pins_array[pin].disabled)
  {
    response = "{\"error\":\"Pin desactivado\"}";
  }
  else
  {
    if (pins_array[pin].status)
    {
      Serial.println("Desactivando pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin));
      digitalWrite(pins_array[pin].pin, pins_array[pin].off_value);
      pins_array[pin].status = false;
      pins_array[pin].result = (char *)"\"Desactivado pin digital\"";
    }
    else
    {
      Serial.println("Pin digital: D" + String(pin) + "-" +
                     String(pins_array[pin].pin) + " ya estaba desactivado.");
    }
    response = pinToJson(pins_array[pin]);
  }
  server.send(200, "application/json", response);
}

String pinToJson(record_type pin)
{
  return "{\"pin\":" + String(pin.index) +
         ", \"description\":" + String(pin.description) +
         ", \"start0\":" + String(pin.start0) +
         ", \"start1\":" + String(pin.start1) +
         ", \"duration0\":" + String(pin.duration0) +
         ", \"duration1\":" + String(pin.duration1) +
         ", \"status\":" + (pin.status ? '1' : '0') +
         ", \"disabled\":" + pin.disabled +
         ", \"result\":" + String(pin.result) +
         "}";
}
