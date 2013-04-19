// WiFly Pachube Client
// Send data to a Pachube Feed
// (Based on Ethernet's WebClient Example)
// (based upon Sparkfun WiFly Web Client example)
// Sparkfun WiFly library updated and can be found here
// https://github.com/jcrouchley/WiFly-Shield
// Built using Arduino IDE V0.22

#include <SPI.h>
#include <WiFly.h>

// Wifly RN-XV (XBee shaped) module connected
//  WiFly Tx to pin 0 (Arduino Rx)
//  WiFly Rx to pin 1 (Arduino Tx)

// using NewSoftSerial V11 beta
// downloaded from here http://arduiniana.org/2011/01/newsoftserial-11-beta/
// this will be included as Software Serial in Arduino IDE 1.0
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);

// Edit credentials.h to provide your own credentials
#include "Credentials.h"

// Using Pachube API V2
WiFlyClient client("api.pachube.com", 80);

void setup() {
  
  pinMode(8,OUTPUT);    // power up the XBee socket
  digitalWrite(8,HIGH);
  // lots of time for the WiFly to start up and also in case I need to stop the transmit
  delay(10000);
  
  Serial.begin(9600);
  mySerial.begin(9600);
  
  WiFly.setUart(&Serial); // Tell the WiFly library that we are not using the SPIUart
  
  mySerial.println("Wifly begin");
  
  WiFly.begin();    // startup the WiFly
  
  mySerial.println("Wifly join");
  
  // Join the WiFi network
  if (!WiFly.join(ssid, passphrase, WEP_MODE)) {
    mySerial.println("Association failed.");
    while (1) {
      // Hang on failure.
    }
  }  

}

uint32_t timeLastUpdated;
int i;
char buff[64];

void loop() {
  if (millis() - timeLastUpdated > TIMETOUPDATE)
  {  // time for the next update
    timeLastUpdated = millis();
    
    // prepare the data to send
    // format (API V2)
    // multiple lines each with <datastreamID>,<datastreamValue>
    // feedID can be the datastream name of the numberic ID
    sprintf(buff,"0,%d\n1,%d",i++,analogRead(0));
    mySerial.println("connecting...");
    if (client.connect()) {
      mySerial.println("connected");
      client.print("PUT /v2/feeds/");  // APIV2
      client.print(PACHUBEFEED);
      client.println(".csv HTTP/1.1");
      client.println("Host: api.pachube.com");
      client.print("X-PachubeApiKey: ");
      client.println(APIKEY);
  
      client.println("User-Agent: Arduino (WiFly RN-XV)");
      client.print("Content-Type: text/csv\nContent-Length: ");
      client.println(strlen(buff));
      client.println("Connection: close");
      client.println();
  
      client.print(buff);
      client.println();
  
    } else {
      mySerial.println("connection failed");
    }

    delay(2000);
    while (client.available()) {
      // TODO verify success (HTTP/1.1 200 OK)
      mySerial.write(client.read());  // display the result
    }
    mySerial.println();
   
    if (client.connected()) {
      mySerial.println("disconnecting.");
      client.stop();
      mySerial.println("disconnected.");
    }
  }
}


