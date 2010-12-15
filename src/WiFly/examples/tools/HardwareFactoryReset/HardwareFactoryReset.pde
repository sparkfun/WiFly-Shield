/*

  HardwareFactoryReset - tool to hardware factory reset a WiFly module

  This code will perform the hardware factory reset sequence on the
  WiFly module on a SparkFun WiFly shield.  

  A factory reset should help when a WiFly module has an unknown baud rate
  or other situations where you can't communicate with the module over
  the serial link. It resets the device to the default configuration.

  Once the reset sequence has been completed you should be able to use
  the Arduino Serial Monitor to enter command mode and send commands in the
  usual manner--as explained in the documentation for the SpiUartTerminal tool.
  
  NOTE: This tool will only work with WiFly shield revisions that have the
        PIO9 and RESET pins connected to the SPI UART IC. The first revision
        which has this feature is identified by the date code "6/15/10" on
        the underside of the PCB.

  Copyright (c) 2010 SparkFun Electronics. http://sparkfun.com LGPL 3.0

 */

#include "WiFly.h" // We use this for the preinstantiated SpiSerial object.

// The GPIO pins on the SPI UART that are connected to WiFly module pins
#define BIT_PIO9  0b00000001 // Hardware factory reset + auto-adhoc
#define BIT_RESET 0b00000010 // Hardware reboot


void setGPIODirection() {
  /*
   */
  SpiSerial.ioSetDirection(BIT_RESET | BIT_PIO9);
}


void hardwareReboot(byte PIO9State = 0) {
  /*
   */
  // NOTE: The WiFly device class implements similar functionality (for reboot
  //       only) but we implement it here slightly differently.
  SpiSerial.ioSetState( (BIT_RESET & ~BIT_RESET) | PIO9State);
  delay(1);
  SpiSerial.ioSetState(BIT_RESET | PIO9State);
}


void readResponse(int timeOut = 0 /* millisecond */) {
  /*
   */

  int target = millis() + timeOut;
  while((millis() < target) || SpiSerial.available() > 0) {
    if (SpiSerial.available()) {
      Serial.print(SpiSerial.read(), BYTE);
    }
  }   
}


#define TOGGLES_REQUIRED 5
#define MS_BETWEEN_TOGGLES 1500

void triggerFactoryReset() {
  /*
   */

  byte state = 0;
  for (int i = 0; i < TOGGLES_REQUIRED; i++) {
    SpiSerial.ioSetState(BIT_RESET | state);  
    state = !state;

    readResponse(MS_BETWEEN_TOGGLES);
  }   
}


void setup() {

  Serial.begin(9600);
  Serial.println("WiFly Shield Hardware Factory Reset Tool");
  Serial.println("----------------------------------------");  
  Serial.println();
    
  Serial.println("This sketch will perform the hardware factory reset sequence");
  Serial.println("on the WiFly module on a SparkFun WiFly shield.");
  Serial.println();
  
  Serial.println("Read the sketch documentation for more details.");
  Serial.println();
 
  Serial.println("Send any character to begin factory reset sequence:");
  
  while(!Serial.available()) {
    // Wait for a character
  }

  Serial.println();
  
  // --------------------------
  
  Serial.println("Attempting to connect to SPI UART...");
  SpiSerial.begin();
  Serial.println("Connected to SPI UART.");
  Serial.println();

  Serial.println("Setting GPIO direction.");
  
  setGPIODirection();
  
  Serial.println();
  Serial.println("First reboot to initiate start of factory reset.");
  Serial.println();
  
  hardwareReboot(BIT_PIO9);

  readResponse(1000);

  Serial.println();
  Serial.println("Toggling pin to trigger factory reset.");
  Serial.println();

  triggerFactoryReset();

  Serial.println();  
  Serial.println("Factory reset should now be complete.");  
  Serial.println();
  
  Serial.println("Rebooting for second and final time.");  
  Serial.println();  
  
  hardwareReboot();

  readResponse(1000);

  Serial.println();  
  Serial.println(">> Finished. <<");    

  // --------------------------
  
  Serial.println();
  
  Serial.println(" * Use $$$ (with no line ending) to enter WiFly command mode. (\"CMD\")");
  Serial.println(" * Then send each command followed by a carriage return.");
  Serial.println();
  
  Serial.println("Waiting for input:");
  Serial.println();      
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
