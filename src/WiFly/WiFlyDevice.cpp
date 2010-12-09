
#include "WiFly.h"

boolean WiFlyDevice::responseMatched(const char *toMatch) {
 /*
  */ 
  boolean matchFound = true;
  
  for (unsigned int offset = 0; offset < strlen(toMatch); offset++) {
    while (!uart.available()) {
      // Wait -- no timeout
    }
    if (uart.read() != toMatch[offset]) {
      matchFound = false;
      break;
    }
  }
  return matchFound;
}


void WiFlyDevice::attemptSwitchToCommandMode() {
  /*
   */
  // Exit command mode if we haven't already
  uart.println("");  
  uart.println("exit");

  // Attempt to enter command mode
  uart.flush();
  uart.print("$$$");
}


void WiFlyDevice::skipRemainderOfResponse() {
  /*
   */
    while (!(uart.available() && (uart.read() == '\n'))) {
      // Skip remainder of response
    }
}  


void WiFlyDevice::waitForResponse(const char *toMatch) {
  /*
   */
   
   // Note: Never exits if the correct response is never found   
   while(!responseMatched(toMatch)) {
     skipRemainderOfResponse();
   }
}



WiFlyDevice::WiFlyDevice(SpiUartDevice& theUart) : uart (theUart) {
  /*
  
    Note: Supplied UART should/need not have been initialised first.
    
   */
  // The WiFly requires the server port to be set between the `reboot`
  // and `join` commands so we go for a "useful" default first.
  serverPort = DEFAULT_SERVER_PORT;
  serverConnectionActive = false;
}

// TODO: Create a constructor that allows a SpiUartDevice (or better a "Stream") to be supplied
//       and/or allow the select pin to be supplied.


void WiFlyDevice::begin() {
  /*
   */
  uart.begin();
  reboot(); // Reboot to get device into known state
  requireFlowControl();
  setConfiguration();
}

// TODO: Create a `begin()` that allows IP etc to be supplied.


void WiFlyDevice::switchToCommandMode() {
  /*
   */

  attemptSwitchToCommandMode();
      
  while (!responseMatched("CMD")) {
    skipRemainderOfResponse(); // TODO: Is this necessary?
    attemptSwitchToCommandMode();
  }
}


void WiFlyDevice::reboot() {
  /*
   */
  switchToCommandMode();

   uart.println("reboot");
   
   waitForResponse("*Reboot*");

   // TODO: Extract information from boot? e.g. version and MAC address

   waitForResponse("*READY*");
}


boolean WiFlyDevice::sendCommand(const char *command,
                                 boolean isMultipartCommand = false,
                                 const char *expectedResponse = "AOK") {
  /*
   */
  uart.print(command);
  
  if (!isMultipartCommand) {
    uart.flush();
    uart.println();
  
    // TODO: Handle other responses
    //       (e.g. autoconnect message before it's turned off,
    //        DHCP messages, and/or ERR etc)
    waitForResponse(expectedResponse);
  }    
  
  return true;
}


void WiFlyDevice::requireFlowControl() {
  /*


    Note: If flow control has been set but not saved then this
          function won't handle it correctly.

    Note: Any other configuration changes made since the last
          reboot will also be saved by this function so this
	  function should ideally be called immediately after a
	  reboot.

   */

  switchToCommandMode();

  // TODO: Reboot here to ensure we get an accurate response and
  //       don't unintentionally save a configuration we don't intend?

  sendCommand("get uart", false, "Flow=0x");

  while (!uart.available()) {
    // Wait to ensure we have the full response
  }

  char flowControlState = uart.read();

  uart.flush();

  if (flowControlState == '1') {
    return;
  }

  // Enable flow control
  sendCommand("set uart flow 1");

  sendCommand("save", false, "Storing in config");

  // Without this (or some delay--but this seemed more useful/reliable)
  // the reboot will fail because we seem to lose the response from the
  // WiFly and end up with something like:
  //     "*ReboWiFly Ver 2.18"
  // instead of the correct:
  //     "*Reboot*WiFly Ver 2.18"
  // TODO: Solve the underlying problem
  sendCommand("get uart", false, "Flow=0x1");

  reboot();
}

void WiFlyDevice::setConfiguration() {
  /*
   */
  switchToCommandMode();

  // TODO: Handle configuration better
  // Turn off auto-connect
  sendCommand("set wlan join 0");

  // TODO: Turn off server functionality until needed
  //       with "set ip protocol <something>"

  // Set server port
  sendCommand("set ip localport ", true);
  // TODO: Handle numeric arguments correctly.
  uart.print(serverPort);
  sendCommand("");

  // Turn off remote connect message
  sendCommand("set comm remote 0");
  
  // Turn off status messages
  // sendCommand("set sys printlvl 0");
  
  // TODO: Change baud rate and then re-connect?
  
  // Turn off RX data echo
  // TODO: Should really treat as bitmask
  // sendCommand("set uart mode 0");
}


boolean WiFlyDevice::join(const char *ssid, const char *passphrase) {
  /*
   */
  // TODO: Handle other authentication methods
  // TODO: Handle escaping spaces/$ in passphrase and SSID
  // TODO: Allow for timeout?

  // TODO: Do this better...
  sendCommand("set wlan passphrase ", true);
  sendCommand(passphrase);

  sendCommand("join ", true);  
  // TODO: Actually detect failure to associate
  if (sendCommand(ssid, false, "Associated!")) {
    // TODO: Extract information from complete response?
    // TODO: Change this to still work when server mode not active
    waitForResponse("Listen on ");
    skipRemainderOfResponse();
    return true;
  }
  return false;
}

#define IP_ADDRESS_BUFFER_SIZE 16 // "255.255.255.255\0"

const char * WiFlyDevice::ip() {
  /*


    The return value is intended to be dropped directly
    into calls to 'print' or 'println' style methods.

   */
  static char ip[IP_ADDRESS_BUFFER_SIZE] = "";
  
  // TODO: Ensure we're not in a connection?

  switchToCommandMode();

  // Version 2.19 of the WiFly firmware has a "get ip a" command but
  // we can't use it because we want to work with 2.18 too.
  sendCommand("get ip", false, "IP=");

  char newChar;
  byte offset = 0;

  // Copy the IP address from the response into our buffer
  while (offset < IP_ADDRESS_BUFFER_SIZE) {
    newChar = uart.read();

    if (newChar == ':') {
      ip[offset] = '\x00';
      break;
    } else if (newChar != -1) {
      ip[offset] = newChar;
      offset++;
    }
  }

  // This handles the case when we reach the end of the buffer
  // in the loop. (Which should never happen anyway.)
  // And hopefully this prevents us from failing completely if
  // there's a mistake above.
  ip[IP_ADDRESS_BUFFER_SIZE-1] = '\x00';

  // This should skip the remainder of the output.
  // TODO: Handle this better?
  waitForResponse("<");
  while (uart.read() != ' ') {
    // Skip the prompt
  }

  // For some reason the "sendCommand" approach leaves the system
  // in a state where it misses the first/next connection so for
  // now we don't check the response.
  // TODO: Fix this
  uart.println("exit");
  //sendCommand("exit", false, "EXIT");

  return ip;
}


// Preinstantiate required objects
SpiUartDevice SpiSerial;
WiFlyDevice WiFly(SpiSerial);

