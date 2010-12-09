/*
 * based on:
 *
 * WiFly Autoconnect Example
 * Copyright (c) 2010 SparkFun Electronics.  All right reserved.
 * Written by Chris Taylor
 *
 * This code was written to demonstrate the WiFly Shield from SparkFun Electronics
 * 
 * This code will initialize and test the SC16IS750 UART-SPI bridge, and automatically
 * connect to a WiFi network using the parameters given in the global variables.
 *
 * http://www.sparkfun.com
 */

#include "WiFly.h"


#include "Credentials.h"


void setup() {

  Serial.begin(9600);
  Serial.println("\n\r\n\rWiFly Shield Terminal Routine");
  
  WiFly.begin();
  
  if (!WiFly.join(ssid, passphrase)) {
    Serial.println("Association failed.");
    while (1) {
      // Hang on failure.
    }
  }
  
  Serial.println("Associated!");
}


void loop() {
  // Terminal routine

  // Always display a response uninterrupted by typing
  // but note that this makes the terminal unresponsive
  // while a response is being received.
  while(SpiSerial.available() > 0) {
    Serial.print(SpiSerial.read(), BYTE);
  }
  
  if(Serial.available()) { // Outgoing data
    SpiSerial.print(Serial.read(), BYTE);
  }
}
