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
 *
 * Further changes here made by Chris Theberge to demo AdHoc mode.
 */

#include <SPI.h>
#include <WiFly.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial( 5, 4);		// used to access the XRF


char ssid[] = "XVAdHoc";

WiFlyServer server(80);

void setup() {

  Serial.begin(9600);
  Serial.println("WiFly Shield AdHoc Example");
  mySerial.begin(9600);
  
  WiFly.setUart(&mySerial); // Tell the WiFly library that we are not using the SPIUart
  // Enable Adhoc mod
  WiFly.begin(true);


if(!WiFly.createAdHocNetwork(ssid)) {
    while(1) {
      Serial.println("Failed to create network, handle error properly?");
    }
  }


  Serial.println("Network Created");
  Serial.print("IP: ");
  Serial.println(WiFly.ip());
  
  Serial.println("Starting Server");
  server.begin();
}


void loop() {
  //
  // Code from the WiFly Server Example
  //
  WiFlyClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean current_line_is_blank = true;
	Serial.println("Got Client");
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
		//Serial.write(c);
        // if we've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so we can send a reply
        if (c == '\n' && current_line_is_blank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          
          // output the value of each analog input pin
          for (int i = 0; i < 6; i++) {
            client.print("analog input ");
            client.print(i);
            client.print(" is ");
            client.print(analogRead(i));
            client.println("<br />");
          }
          break;
        }
        if (c == '\n') {
          // we're starting a new line
          current_line_is_blank = true;
        } else if (c != '\r') {
          // we've gotten a character on the current line
          current_line_is_blank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(100);
    client.stop();
  }
}
