/*

  SpiUartTerminal - tool to help troubleshoot problems with WiFly shield

  This code will initialise and test the SC16IS750 UART-SPI bridge then enable you
  to send commands to the WiFly module.

  Copyright (c) 2010 SparkFun Electronics. http://sparkfun.com LGPL 3.0

 */

#include <SPI.h>
#include <WiFly.h>

void setup() {

  Serial.begin(9600);
  Serial.println("SPI UART on WiFly Shield terminal tool");
  Serial.println("--------------------------------------");  
  Serial.println();
  Serial.println("This is a tool to help you troubleshoot problems with the WiFly shield.");
  Serial.println("For consistent results unplug & replug power to your Arduino and WiFly shield.");
  Serial.println("(Ensure the serial monitor is not open when you remove power.)");  
  Serial.println();
  
  Serial.println("Attempting to connect to SPI UART...");
  SpiSerial.begin();
  Serial.println("Connected to SPI UART.");
  Serial.println();
  
  Serial.println(" * Use $$$ (with no line ending) to enter WiFly command mode. (\"CMD\")");
  Serial.println(" * Then send each command followed by a carriage return.");
  Serial.println();
  
  Serial.println("Waiting for input.");
  Serial.println();    
  
}


void loop() {
  // Terminal routine

  // Always display a response uninterrupted by typing
  // but note that this makes the terminal unresponsive
  // while a response is being received.
  while(SpiSerial.available() > 0) {
    Serial.write(SpiSerial.read());
  }
  
  if(Serial.available()) { // Outgoing data
    SpiSerial.write(Serial.read());
  }
}
