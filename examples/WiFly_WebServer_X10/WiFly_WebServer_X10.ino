/*
 * Web Server
 *
 * This is a simple web server that takes HTTP GET requests and translates them to X10 commands.
 *
 * The X10 webserver was written by Chris Theberge
 *
 * The goal is to expand and use this HTTP Parser from Ryan Dhal (https://github.com/ry/http-parser)
 *
 * See the TODO notes for some clean up things.
 */
#include <X10.h>
#include <x10constants.h>
#include <SPI.h>
#include <WiFly.h>
#include <string.h>
#include <stddef.h>

#include "Credentials.h"

#define zcPin 2
#define dataPin 3

const char delimiters[] = "/";

// set up a new x10 instance:
x10 myHouse =  x10(zcPin, dataPin);
WiFlyServer server(80);

void setup() {
  Serial.begin(9600);
  Serial.println("X10 Webserver");

  WiFly.begin();

  if (!WiFly.join(ssid, passphrase)) {
    while (1) {
      // Hang on failure.
    }
  }

  Serial.print("IP: ");
  Serial.println(WiFly.ip());

  server.begin();
}

#define MAX_COMMAND_LENGTH 50

void loop() {
  WiFlyClient client = server.available();
  if (client) {

    Serial.print("Client Found...\n");

    char buffer[512];
    char get_url[MAX_COMMAND_LENGTH];
    char house[MAX_COMMAND_LENGTH];
    char command[MAX_COMMAND_LENGTH];
    // A http request ends with a blank line
    boolean current_line_is_blank = true;
    boolean parsed_ok = false;
    int buffPos = 0;

    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();
        buffer[buffPos++] = c;
        Serial.print(c);

        // if we've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so we can send a reply
        if (c == '\n' && current_line_is_blank) {

          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();

          sscanf(buffer, "GET %s HTTP/1.1", get_url);

          char *cp = strtok(get_url, delimiters);
          if(cp != NULL)
          {
            strcpy(house, cp); 
            cp = strtok(NULL, delimiters);
          }

          if(cp != NULL)
          {
            strcpy(command, cp);
            parsed_ok = true;
          }

          if(parsed_ok)
          {
            byte houseCode = A;
            byte numCode;

            //
            // TODO: Change this to a Null-terminated List and replace
            // these calls with a search function that automatically calls a function when an action is found.
            //
            // Use a Structure with a Command and a function pointer and automatically search the list.
            //

            if(strcmp(command, "ALL_LIGHTS_ON") == 0)
            {
              numCode = ALL_LIGHTS_ON;
            }
            else if(strcmp(command, "ALL_LIGHTS_OFF") == 0)
            {
              numCode = ALL_LIGHTS_OFF;
            }
            else if(strcmp(command, "UNIT_1") == 0)
            { 
              numCode = UNIT_1;
            }
            else if(strcmp(command, "UNIT_2") == 0)
            { 
              numCode = UNIT_2;
            }
            else if(strcmp(command, "UNIT_3") == 0)
            { 
              numCode = UNIT_3;
            }
            else if(strcmp(command, "UNIT_4") == 0)
            { 
              numCode = UNIT_4;
            }
            else if(strcmp(command, "ON") == 0)
            {
              numCode = ON;
            }
            else if(strcmp(command, "OFF") == 0)
            {
              numCode = OFF;
            } 
            else if(strcmp(command, "DIM") == 0)
            {
              numCode = DIM;
            }
            else if(strcmp(command, "BRIGHT") == 0)
            {
              numCode = BRIGHT;
            }

            myHouse.write(houseCode, numCode, 3);
          }

          // Output something for the client...
          client.print("Request Received");

          break;
        }

        if (c == '\n') {
          // we're starting a new line
          current_line_is_blank = true;
        } 
        else if (c != '\r') {
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




