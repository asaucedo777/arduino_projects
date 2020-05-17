#ifndef _REMOTEXY_H_
#define _REMOTEXY_H_

#include <inttypes.h>
#include "classes/RemoteXY_Lib.h"
#include "modules/esp8266.h"

CRemoteXY *remotexy;

#define RemoteXY_Handler() remotexy->handler()
#define RemoteXY_CONF const PROGMEM RemoteXY_CONF_PROGMEM

#endif
