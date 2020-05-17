#ifndef _REMOTEXY_MOD_ESP8266_H_
#define _REMOTEXY_MOD_ESP8266_H_

#include "../classes/RemoteXY_AT.h"

#define REMOTEXY_ESP8266_MAX_SEND_BYTES 2048
#define REMOTEXY_ESP8266_MODULETEST_TIMEOUT 10000

class CRemoteXY : public CRemoteXY_AT
{

protected:
  char *wifiSsid;
  char *wifiPassword;
  uint16_t port;
  char connectCannel;
  uint16_t connectAvailable;
  uint16_t freeAvailable;
  uint16_t sendBytesAvailable;
  uint16_t sendBytesLater;
  uint32_t moduleTestTimeout;

public:
  CRemoteXY(const void *_conf, void *_var, const char *_accessPassword, HardwareSerial *_serial, long _serialSpeed, const char *_wifiSsid, const char *_wifiPassword, uint16_t _port)
  {
    initSerial(_serial, _serialSpeed);
    initAT();
    wifiSsid = (char *)_wifiSsid;
    wifiPassword = (char *)_wifiPassword;
    port = _port;
    connectCannel = 0;
    connectAvailable = 0;
    freeAvailable = 0;
    sendBytesAvailable = 0;
    sendBytesLater = 0;
    init(_conf, _var, _accessPassword);
    moduleTestTimeout = millis();
  }

protected:
  //override AT
  void readyAT()
  {
    setModule();
  }

  //override AT
  void connectAT()
  {
    if (connectCannel == 0)
    {
      connectCannel = *(params[0]);
      connectAvailable = 0;
    }
  };

  //override AT
  void closedAT()
  {
    if (connectCannel == *(params[0]))
    {
      connectCannel = 0;
    }
  }

  //override AT
  void inputDataAT()
  {
    uint16_t size;
    size = getATParamInt(1);
    if (connectCannel == *(params[0]))
    {
      connectAvailable = size;
    }
    else
    {
      freeAvailable = size;
    }
  }

  uint8_t initModule()
  {
    uint8_t tryCount = 20;
    while (--tryCount > 0)
    {
      sendATCommand("AT", 0);
      if (waitATAnswer(AT_ANSWER_OK, AT_DELAY_1_SEG))
      {
        break;
      }
    }
    if (tryCount == 0)
    {
      return 0;
    }
    sendATCommand("AT+RST", 0);
    if (!waitATAnswer(AT_ANSWER_OK, AT_DELAY_1_SEG))
    {
      return 0;
    }
    if (!waitATAnswer(AT_MESSAGE_READY, AT_DELAY_5_SEG))
    {
      return 0;
    }
    return setModule();
  }

  uint8_t setModule()
  {
    // serial->write("setModule...");
    char sport[6];
    rxy_itos(port, sport);
    char stimeout[6];
    rxy_itos(REMOTEXY_SERVER_TIMEOUT / 1000, stimeout);
    connectCannel = 0;
    connectAvailable = 0;
    sendATCommand("ATE0", 0);
    if (!waitATAnswer(AT_ANSWER_OK, AT_DELAY_1_SEG))
    {
      return 0;
    }
    sendATCommand("AT+CWMODE=2", 0);
    if (!waitATAnswer(AT_ANSWER_OK, AT_DELAY_2_SEG))
    {
      return 0;
    }
    sendATCommand("AT+CWDHCP=0,1", 0);
    if (!waitATAnswer(AT_ANSWER_OK, AT_DELAY_2_SEG))
    {
      return 0;
    }
    char crypt[2] = {*wifiPassword ? '4' : '0', 0};
    sendATCommand("AT+CWSAP=\"", wifiSsid, "\",\"", wifiPassword, "\",10,", crypt, 0);
    if (!waitATAnswer(AT_ANSWER_OK, AT_DELAY_5_SEG))
    {
      return 0;
    }
    sendATCommand("AT+CIPMODE=0", 0);
    if (!waitATAnswer(AT_ANSWER_OK, AT_DELAY_2_SEG))
    {
      return 0;
    }
    sendATCommand("AT+CIPMUX=1", 0);
    if (!waitATAnswer(AT_ANSWER_OK, AT_DELAY_1_SEG))
      return 0;
    sendATCommand("AT+CIPSERVER=1,", sport, 0);
    if (!waitATAnswer(AT_ANSWER_OK, AT_DELAY_1_SEG))
    {
      return 0;
    }
    sendATCommand("AT+CIPSTO=", stimeout, 0);
    if (!waitATAnswer(AT_ANSWER_OK, AT_DELAY_1_SEG))
    {
      return 0;
    }
    moduleTestTimeout = millis();
    return 1;
  }

  void handlerModule()
  {
    while (serial->available() > 0)
    {
      if (connectAvailable)
      {
        break;
      }
      if (freeAvailable)
      {
        serial->read();
        freeAvailable--;
      }
      else
      {
        readATMessage();
      }
      moduleTestTimeout = millis();
    }
    if (millis() - moduleTestTimeout > REMOTEXY_ESP8266_MODULETEST_TIMEOUT)
    {
      moduleTestTimeout = millis();
      if (testATecho() == 2)
      {
        setModule();
      }
    }
  }

  void sendStart(uint16_t len)
  {
    char s[8];
    if (connectCannel)
    {
      sendBytesLater = 0;
      if (len > REMOTEXY_ESP8266_MAX_SEND_BYTES)
      {
        sendBytesLater = len - REMOTEXY_ESP8266_MAX_SEND_BYTES;
        len = REMOTEXY_ESP8266_MAX_SEND_BYTES;
      }
      sendBytesAvailable = len;
      rxy_itos(len, s + 2);
      *s = connectCannel;
      *(s + 1) = ',';
      sendATCommand("AT+CIPSEND=", s, 0);
      if (!waitATAnswer(AT_ANSWER_GO, 1000))
      {
        sendBytesAvailable = 0;
      }
    }
  }

  void sendByte(uint8_t b)
  {
    if (sendBytesAvailable)
    {
      serial->write(b);
      sendBytesAvailable--;
      if (!sendBytesAvailable)
      {
        waitATAnswer(AT_ANSWER_SEND_OK, 1000);
        if (sendBytesLater)
        {
          sendStart(sendBytesLater);
        }
      }
    }
  }

  uint8_t receiveByte()
  {
    uint8_t b;
    if (connectAvailable)
    {
      if (serial->available() > 0)
      {
        connectAvailable--;
        b = serial->read();
        return b;
      }
    }
    return 0;
  }

  uint8_t availableByte()
  {
    if (connectAvailable)
    {
      return serial->available() > 0;
    }
    return 0;
  }
};

// ninguna instancia del constructor "CRemoteXY::CRemoteXY" coincide con la lista de argumentos-- los tipos de argumento son : (const uint8_t[37], struct<unnamed> *, const char[1], Serial_ *, int, const char[9], const char[9], int)

// CRemoteXY::CRemoteXY(const void *_conf, void *_var, const char *_accessPassword, HardwareSerial *_serial, long _serialSpeed, const char *_wifiSsid, const char *_wifiPassword, uint16_t _port)

#define RemoteXY_Init() remotexy = new CRemoteXY(RemoteXY_CONF_PROGMEM, &RemoteXY, "", &REMOTEXY_SERIAL, REMOTEXY_SERIAL_SPEED, REMOTEXY_WIFI_SSID, REMOTEXY_WIFI_PASSWORD, REMOTEXY_SERVER_PORT)

#endif
