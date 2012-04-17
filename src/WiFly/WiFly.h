#ifndef __WIFLY_H__
#define __WIFLY_H__

#include "SpiUart.h"

#include "WiFlyDevice.h"

#include "Client.h"
#include "Server.h"

// Join modes
#define WEP_MODE false
#define WPA_MODE true

// Antenna types
#define INTERNAL_ANTENNA 0
#define EXTERNAL_ANTENNA 1

// Configuration options
#define WIFLY_BAUD 1
#define ANTENNA_TYPE 2

// TODO: Don't make this extern
// TODO: How/whether to allow sending of arbitrary data/commands over serial?
// NOTE: Only the WiFly object is intended for external use
extern SpiUartDevice SpiSerial;

extern WiFlyDevice WiFly;

#endif

