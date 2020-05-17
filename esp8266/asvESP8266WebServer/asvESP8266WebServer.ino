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
const int BAUD_RATE = 115200;
const int DELAY_CONNECT = 20;
const char *HOST_NAME = "esp8266";
const char *SERVER_NTP_1 = "pool.ntp.org";
const char *SERVER_NTP_2 = "time.nist.gov";

ESP8266WebServer server(80);
boolean apagado = false;
int connectionTime = 0;

typedef struct record_type {
  int pin;
  boolean status;
};
record_type pins_array[9];

void setup(void)
{
  pins_array[0] = (record_type){16, false};
  pins_array[1] = (record_type){5, false};
  pins_array[2] = (record_type){4, false};
  pins_array[3] = (record_type){0, false};
  pins_array[4] = (record_type){2, false};
  pins_array[5] = (record_type){14, false};
  pins_array[6] = (record_type){12, false};
  pins_array[7] = (record_type){1, false};
  pins_array[8] = (record_type){15, false};

  Serial.begin(BAUD_RATE);
  while (!Serial) { ; }
  Serial.print("Serial BAUD_RATE: ");
  Serial.println(BAUD_RATE);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    if (connectionTime % 500 == 0)
    {
      Serial.print(" ");
    }
    else
    {
      Serial.print(".");
    }
    delay(DELAY_CONNECT);
    connectionTime += DELAY_CONNECT;
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connection time: ");
  Serial.print(connectionTime);
  Serial.print(" ms\nConnected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin(HOST_NAME))
  {
    Serial.print("MDNS host name: ");
    Serial.println(HOST_NAME);
  }

  server.on("/test", handleTest);
  server.on("/LED_BUILTIN/SWITCH", handleLedBuiltinSwitch);
  server.on("/LED_BUILTIN/ON", handleLedBuiltinOn);
  server.on("/LED_BUILTIN/OFF", handleLedBuiltinOff);
  server.on("/TIME", handleTime);
  server.on("/DIGITAL_PIN/SWITCH", handleDigitalPinSwitch);
  server.on("/DIGITAL_PIN/ON", handleDigitalPinOn);
  server.on("/DIGITAL_PIN/OFF", handleDigitalPinOff);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  configTime(2 * 3600, 0, SERVER_NTP_1, SERVER_NTP_2);
  Serial.println("Waiting for time..");
  while (!time(nullptr))
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Time configured.");

  pinMode(pins_array[1].pin, OUTPUT);
  digitalWrite(pins_array[1].pin, LOW);
  Serial.println("Pines digitales inicializados.");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("LED_BUILTIN ON");
}

void loop(void)
{
  server.handleClient();
  MDNS.update();
}

void handleTest()
{
  server.send(200, "text/plain", "this works as well");
}

void handleDigitalPinSwitch()
{
  String pin = server.arg(String("pin"));
  if (pin != nullptr && pin != "")
  {
    digitalPinSwitch(pin.toInt());
  } else {
    Serial.println("No se ha recuperado el parametro");
  }
}

void handleDigitalPinOn()
{
  String pin = server.arg(String("pin"));
  if (pin != nullptr && pin != "")
  {
    digitalPinOn(pin.toInt());
  } else {
    Serial.println("No se ha recuperado el pin a activar");
  }
}

void handleDigitalPinOff()
{
  String pin = server.arg(String("pin"));
  if (pin != nullptr && pin != "")
  {
    digitalPinOff(pin.toInt());
  } else {
    Serial.println("No se ha recuperado el pin a desactivar");
  }
}

void digitalPinSwitch(int pin)
{
  if (pins_array[pin].status)
  {
    Serial.println("Desactivando pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin));
    digitalWrite(pins_array[pin].pin, LOW);
    pins_array[pin].status = false;
    server.send(200, "text/plain", "Pin digital D" +  String(pin) + " SWITCH A apagado");
  }
  else
  {
    Serial.println("Activando pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin));
    digitalWrite(pins_array[pin].pin, HIGH);
    pins_array[pin].status = true;
    server.send(200, "text/plain", "Pin digital D" +  String(pin) + " SWITCH A encendido");
  }
}

void digitalPinOn(int pin)
{
  if (pins_array[pin].status)
  {
    Serial.println("Pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin) + " ya estaba activado.");
    server.send(200, "text/plain", "Pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin) + " ya está activado.");
  }
  else
  {
    Serial.println("Activando pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin));
    digitalWrite(pins_array[pin].pin, HIGH);
    pins_array[pin].status = true;
    server.send(200, "text/plain", "Pin digital D" +  String(pin) + " activado");
  }
}

void digitalPinOff(int pin)
{
  if (pins_array[pin].status)
  {
    Serial.println("Desactivando pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin));
    digitalWrite(pins_array[pin].pin, LOW);
    pins_array[pin].status = false;
    server.send(200, "text/plain", "Pin digital D" +  String(pin) + " desactivado");
  }
  else
  {
    Serial.println("Pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin) + " ya estaba desactivado.");
    server.send(200, "text/plain", "Pin digital: D" + String(pin) + "-" + String(pins_array[pin].pin) + " ya está desactivado.");
  }
}

void handleLedBuiltinSwitch()
{
  if (apagado)
  {
    Serial.println("Encenciendo LED");
    digitalWrite(LED_BUILTIN, LOW);
    apagado = false;
    server.send(200, "text/plain", "LED_BUILTIN SWITCH A encendido");
  }
  else
  {
    Serial.println("Apagando LED");
    digitalWrite(LED_BUILTIN, HIGH);
    apagado = true;
    server.send(200, "text/plain", "LED_BUILTIN SWITCH A apagado");
  }
}

void handleLedBuiltinOn()
{
  if (apagado)
  {
    Serial.println("Encenciendo LED");
    digitalWrite(LED_BUILTIN, LOW);
    apagado = false;
    server.send(200, "text/plain", "LED_BUILTIN encendido");
  }
  else
  {
    Serial.println("LED ya está encendido.");
    server.send(200, "text/plain", "LED_BUILTIN ya está encendido");
  }
}

void handleLedBuiltinOff()
{
  if (apagado)
  {
    Serial.println("LED ya está apagado.");
    server.send(200, "text/plain", "LED_BUILTIN ya está apagado");
  }
  else
  {
    Serial.println("Apagando LED.");
    digitalWrite(LED_BUILTIN, HIGH);
    apagado = true;
    server.send(200, "text/plain", "LED_BUILTIN apagado");
  }
}

void handleTime()
{
  time_t now = time(nullptr);
  char buff[20];
  strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
  Serial.print("Date-time: ");
  Serial.println(buff);
  server.send(200, "text/plain", buff);
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
