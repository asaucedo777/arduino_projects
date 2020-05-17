// AsvEsp8266.ino
#include "RemoteXY.h"

const int PORT_LED = 2;

#define REMOTEXY_SERIAL Serial
#define REMOTEXY_SERIAL_SPEED 115200
#define REMOTEXY_WIFI_SSID "RemoteXY"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377

#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] = {
    255, 2, 0, 0,
    0, 30, 0, 10,
    13, 0, 2, 0,
    70, 26, 22, 11,
    2, 26, 31, 31,
    79, 78, 0, 79,
    70, 70, 0, 1,
    0, 17, 25, 12,
    12, 2, 31, 88,
    0};
struct
{
  uint8_t switch_1;
  uint8_t connect_flag;
} RemoteXY;
#pragma pack(pop)

#define LED_13 13
void setup()
{
  Serial.begin(REMOTEXY_SERIAL_SPEED);
  Serial.print("Start serial");
  pinMode(PORT_LED, OUTPUT);
  RemoteXY_Init();
}

void loop()
{
  digitalWrite(PORT_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(PORT_LED, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
  // Serial.println("s");
  delay(1000);                       // wait for a second
  RemoteXY_Handler();
  digitalWrite(PORT_LED, (RemoteXY.switch_1 == 0) ? LOW : HIGH);
  // do not call delay()
}