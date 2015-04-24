#include <SPI.h>
#include <WiFly.h>
#include "String.h"

#include "Credentials.h"

WiFlyClient client("api.thingspeak.com", 80);

#include <string.h>
#include <stdlib.h>
#include <stdio.h> 

#define UPDATE_INTERVAL            60000    // if the connection is good wait 10 seconds before updating again - should not be less than 5
#define RETRY_INTERVAL						 20000    // Wait this ms before trying to connect to the network again
#define THINGSPEAK_API_KEY  "ABCDEFGHIJKLM" // fill in your API key 

static uint32_t timeLastUpdated=0;
int connTries=0;

bool checkNetwork()
{
  const char * status=WiFly.getConnectionStatus();
  if(status[1]=='3')
  {
    return true;
  }
  else
  {
    return false;
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Thingspeak example!");
  Serial.println("Will send some data to Thingspeak every UPDATE_INTERVAL ms");
  WiFly.begin();
}

void loop()
{
  if (millis() - timeLastUpdated > UPDATE_INTERVAL)
  { // time for the next update
    if(!checkNetwork())
    {
      Serial.println("Not connected to the wifi network.");
      while(!WiFly.join(ssid,passphrase))
      {
        Serial.println("Joining the network failed. Retrying in RETRY_INTERVAL.");
        delay(RETRY_INTERVAL);
        connTries++;
      }
      Serial.println("Joined!");
    }
    
    Serial.print("Time from last update: ");
    Serial.println(millis() - timeLastUpdated);
    float temp=12.3; // Replace this with the value you want to send to field1. I used a LM35 temperature sensor.
    Serial.print("Current temperature: ");
    Serial.println(temp);
    // Convert a float to a string with style
    char buffer[14];
    dtostrf(temp,7,2,buffer);
    String stemp(buffer);
    stemp.trim();
    // float is now in the string stemp
    timeLastUpdated = millis();
    String sret(connTries);
    if(client.connect())
    {
      Serial.println("connected... Sending data to Thingspeak.");
      thingspeak_out("field1="+stemp+"&field2="+sret);
    }
    else
    {
      Serial.println("Error connecting. Will try again at next interval.");
    }
  }
  delay(UPDATE_INTERVAL);
}

void thingspeak_out(String _data)
{
	client.println("POST /update HTTP/1.1"); 
	client.println("Host: api.thingspeak.com");
	client.println("Connection: close");
	client.print("X-THINGSPEAKAPIKEY: ");
	client.println(THINGSPEAK_API_KEY);
	//client.println("User-Agent: Arduino (WiFly RN-XV)");
	client.print("Content-Type: application/x-www-form-urlencoded\n");
	client.print("Content-Length: ");
	client.print(_data.length());
	client.print("\n\n");
	client.println(_data);

  delay(2000); // Give the server some time to answer back
  
  while (client.available()) 
  {
    // TODO verify success (HTTP/1.1 200 OK)
    Serial.print(client.readChar());  // display the result
  }
  Serial.println();
  
  if (client.connected()) 
  {
    Serial.println("disconnecting...");
    client.stop();
  } 
}