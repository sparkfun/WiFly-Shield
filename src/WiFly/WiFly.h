#ifndef __WIFLY_H__
#define __WIFLY_H__

#include "SpiUart.h"

#include "WiFlyDevice.h"

#include "Client.h"
#include "Server.h"

#define WEP_MODE false
#define WPA_MODE true

// TODO: Don't make this extern
// TODO: How/whether to allow sending of arbitrary data/commands over serial?
// NOTE: Only the WiFly object is intended for external use
extern SpiUartDevice SpiSerial;

extern WiFlyDevice WiFly;

#endif

