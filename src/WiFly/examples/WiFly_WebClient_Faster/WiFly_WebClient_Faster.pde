
// (Based on Ethernet's WebClient Example)

#include "WiFly.h"


#include "Credentials.h"


byte server[] = { 66, 249, 89, 104 }; // Google

//Client client(server, 80);

Client client("google.com", 80);

void setup() {
  
  Serial.begin(115200);
  Serial.println("WebClient example at 38400 baud.");

  WiFly.begin();

  if (!WiFly.join(ssid, passphrase)) {
    Serial.println("Association failed.");
    while (1) {
      // Hang on failure.
    }
  }  

  WiFly.configure(WIFLY_BAUD, 38400);

  Serial.println("connecting...");

  if (client.connect()) {
    Serial.println("connected");
    client.println("GET /search?q=arduino HTTP/1.0");
    client.println();
  } else {
    Serial.println("connection failed");
  }
  
}

int count = 0;

void loop() {
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
    count++;
    if (count > 80) {
      count = 0;
      Serial.println();
    }
  }
  
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    for(;;)
      ;
  }
}


