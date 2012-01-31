// WiFly Pachube Client
// Send data to a Pachube Feed
// (Based on Ethernet's WebClient Example)
// (based upon Sparkfun WiFly Web Client example)
// Sparkfun WiFly library updated and can be found here
// https://github.com/jcrouchley/WiFly-Shield
// Built using Arduino IDE V0.22

#include "WiFly.h"

// using NewSoftSerial V11 beta
// downloaded from here http://arduiniana.org/2011/01/newsoftserial-11-beta/
// this will be included as Software Serial in Arduino IDE 1.0
#include <SoftwareSerial.h>

// Wifly RN-XV (XBee shaped) module connected
//  WiFly Tx to pin 2 (SoftSerial Rx)
//  WiFly Rx to pin 3 (SoftSerial Tx)
SoftwareSerial mySerial(2, 3);


// Edit credentials.h to provide your own credentials
#include "Credentials.h"

// Using Pachube API V2
WiFlyClient client("api.pachube.com", 80);

void setup() {
  
  pinMode(8,OUTPUT);
  digitalWrite(8,HIGH);
  // lots of time for the WiFly to start up and also in case I need to stop the transmit
  delay(10000);
  
  Serial.begin(115200);  // nice and fast
  mySerial.begin(9600);  // default WiFly baud rate - good enough for this

  WiFly.setUart(&mySerial); // Tell the WiFly library that we are not using the SPIUart
  
  Serial.println("Wifly begin");
  
  WiFly.begin();    // startup the WiFly
  
  Serial.println("Wifly join");
  
  // Join the WiFi network
  if (!WiFly.join(ssid, passphrase, WEP_MODE)) {
    Serial.println("Association failed.");
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
    Serial.println("connecting...");
    if (client.connect()) {
      Serial.println("connected");
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
      Serial.println("connection failed");
    }

    delay(2000);
    while (client.available()) {
      // TODO verify success (HTTP/1.1 200 OK)
      Serial.write(client.read());  // display the result
    }
    Serial.println();
   
    if (client.connected()) {
      Serial.println("disconnecting.");
      client.stop();
      Serial.println("disconnected.");
    }
  }
}


