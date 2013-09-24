
#include "SPI.h"
#include "WiFly.h"
#include "Credentials.h"

char nma_host[]="www.notifymyandroid.com";
String nma_key="insert_your_nma_apikey_here";
 
WiFlyClient client( nma_host , 80 );
 
bool tryToJoin()
{
  Serial.println("Wifly trying to join.");
  if (!WiFly.join(ssid,passphrase)) 
  {
    return false;
  }  
  else
  {
    return true;
  }
}
 
void setup()
{
  Serial.begin( 9600 ); 
  WiFly.begin();
  while(tryToJoin()!=true)
  {
    Serial.println("Association failed. Waiting 5 seconds.");
    delay(5000);
  }
  Serial.println("Joined!");
}

static int loops=0;
static bool verifySent=false;
static bool notifySent=false;
static long tv=0;

void loop()
{
  loops++;
  if(notifySent==false && verifySent==true && millis()>(tv+5000))
  {
    if(client.connected() || client.connect()) 
    {
      Serial.println("Sending notify URL to NMA");
      String data = "apikey="+nma_key+"&application=arduino_wifly&event=test&description=yeah+baby";
      client.println( "POST /publicapi/notify HTTP/1.1" );
      client.println( "Host: www.notifymyandroid.com" );
      client.println( "Content-Type: application/x-www-form-urlencoded" );
      client.println( "Connection: close" );
      client.print( "Content-Length: " );
      client.println( data.length() );
      client.println();
      client.print( data );
      client.println();
      notifySent=true;
    } 
    else
    {
      Serial.println("Error connecting with remote host");
    }
  }

  if(verifySent==false)
  {
    if(!client.connected() && client.connect()) 
    {
      Serial.println("Sending verify URL to NMA");
      client.println("GET /publicapi/verify?apikey="+nma_key+" HTTP/1.1");
      client.println("Host: www.notifymyandroid.com");
      client.println();
      verifySent=true;
      tv=millis();
    }
    else
    {
      Serial.println("Error connecting with remote host");
    }
  }
  
  while(client.available())
  {
    char c = client.read();
    Serial.print(c);
  }
}

