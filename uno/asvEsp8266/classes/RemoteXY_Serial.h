#ifndef _REMOTEXY_SERIAL_H_
#define _REMOTEXY_SERIAL_H_

#include "RemoteXY_API.h"

class CRemoteXY_Serial : public CRemoteXY_API
{

public:
  HardwareSerial *serial;
  void initSerial(HardwareSerial *_serial, long _serialSpeed)
  {
    serial = _serial;
    serial->begin(_serialSpeed);
  }
};

#endif
