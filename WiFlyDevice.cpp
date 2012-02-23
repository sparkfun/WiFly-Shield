
#include "WiFly.h"

#define DEBUG_LEVEL 0

#include "Debug.h"


boolean WiFlyDevice::findInResponse(const char *toMatch,
                                    unsigned int timeOut = 1000) {
  /*

   */

  // TODO: Change 'sendCommand' to use 'findInResponse' and have timeouts,
  //       and then use 'sendCommand' in routines that call 'findInResponse'?

  // TODO: Don't reset timer after successful character read? Or have two
  //       types of timeout?

  int byteRead;

  unsigned long timeOutTarget; // in milliseconds

  DEBUG_LOG(1, "Entered findInResponse");
  DEBUG_LOG(2, "Want to match:");
  DEBUG_LOG(2, toMatch);
  DEBUG_LOG(3, "Found:");

  for (unsigned int offset = 0; offset < strlen(toMatch); offset++) {

    // Reset after successful character read
    timeOutTarget = millis() + timeOut; // Doesn't handle timer wrapping

    while (!uart->available()) {
      // Wait, with optional time out.
      if (timeOut > 0) {
        if (millis() > timeOutTarget) {
          return false;
        }
      }
      delay(1); // This seems to improve reliability slightly
    }

    // We read this separately from the conditional statement so we can
    // log the character read when debugging.
    byteRead = uart->read();

    delay(1); // Removing logging may affect timing slightly

    DEBUG_LOG(5, "Offset:");
    DEBUG_LOG(5, offset);
    DEBUG_LOG(3, (char) byteRead);
    DEBUG_LOG(4, byteRead);

    if (byteRead != toMatch[offset]) {
      offset = 0;
      // Ignore character read if it's not a match for the start of the string
      if (byteRead != toMatch[offset]) {
        offset = -1;
      }
      continue;
    }
  }
  DEBUG_LOG(2, "Response found");

  return true;
}



boolean WiFlyDevice::responseMatched(const char *toMatch) {
  /*
   */
  boolean matchFound = true;
  unsigned long timeout;

DEBUG_LOG(3, "Entered responseMatched");
  for (unsigned int offset = 0; offset < strlen(toMatch); offset++) {
    timeout = millis();
    while (!uart->available()) {
      // Wait, with optional time out.
      if (millis() - timeout > 5000) {
          return false;
        }
      delay(1); // This seems to improve reliability slightly
    }
DEBUG_LOG(3,(char)uart->peek());
    if (uart->read() != toMatch[offset]) {
      matchFound = false;
      break;
    }
  }
  return matchFound;
}



#define COMMAND_MODE_ENTER_RETRY_ATTEMPTS 5

#define COMMAND_MODE_GUARD_TIME 250 // in milliseconds

boolean WiFlyDevice::enterCommandMode(boolean isAfterBoot) {
  /*

   */

  DEBUG_LOG(1, "Entered enterCommandMode");

  // Note: We used to first try to exit command mode in case we were
  //       already in it. Doing this actually seems to be less
  //       reliable so instead we now just ignore the errors from
  //       sending the "$$$" in command mode.

  for (int retryCount = 0;
       retryCount < COMMAND_MODE_ENTER_RETRY_ATTEMPTS;
       retryCount++) {

    // At first I tried automatically performing the
    // wait-send-wait-send-send process twice before checking if it
    // succeeded. But I removed the automatic retransmission even
    // though it makes things  marginally less reliable because it speeds
    // up the (hopefully) more common case of it working after one
    // transmission. We also now have automatic-retries for the whole
    // process now so it's less important anyway.

    if (isAfterBoot) {
      delay(1000); // This delay is so characters aren't missed after a reboot.
    }

    delay(COMMAND_MODE_GUARD_TIME);

    uart->print(F("$$$"));

    delay(COMMAND_MODE_GUARD_TIME);

    // We could already be in command mode or not.
    // We could also have a half entered command.
    // If we have a half entered command the "$$$" we've just added
    // could succeed or it could trigger an error--there's a small
    // chance it could also screw something up (by being a valid
    // argument) but hopefully it's not a general issue.  Sending
    // these two newlines is intended to clear any partial commands if
    // we're in command mode and in either case trigger the display of
    // the version prompt (not that we actually check for it at the moment
    // (anymore)).

    // TODO: Determine if we need less boilerplate here.

    uart->println();
    uart->println();

    // TODO: Add flush with timeout here?

    // This is used to determine whether command mode has been entered
    // successfully.
    // TODO: Find alternate approach or only use this method after a (re)boot?
    uart->println(F("ver"));

    if (findInResponse("\r\nWiFly Ver", 1000)) {
      // TODO: Flush or leave remainder of output?
      return true;
    }
  }
  return false;
}



void WiFlyDevice::skipRemainderOfResponse() {
  /*
   */

  DEBUG_LOG(3, "Entered skipRemainderOfResponse");

    while (!(uart->available() && (uart->read() == '\n'))) {
      // Skip remainder of response
    }
}


void WiFlyDevice::waitForResponse(const char *toMatch) {
  /*
   */
   // Note: Never exits if the correct response is never found
   findInResponse(toMatch);
 
}



WiFlyDevice::WiFlyDevice(SpiUartDevice& theUart) : SPIuart (theUart) {
  /*

    Note: Supplied UART should/need not have been initialised first.

   */
  bDifferentUart = 0;
  uart = &SPIuart;
  // The WiFly requires the server port to be set between the `reboot`
  // and `join` commands so we go for a "useful" default first.
  serverPort = DEFAULT_SERVER_PORT;
  serverConnectionActive = false;
}

// TODO: Create a constructor that allows a SpiUartDevice (or better a "Stream") to be supplied
//       and/or allow the select pin to be supplied.


void  WiFlyDevice::setUart(Stream* newUart)
{
  bDifferentUart = 1;
  uart = newUart;
}

void WiFlyDevice::begin() {
	begin(false);
}

void WiFlyDevice::begin(boolean adhocMode) {
  /*
   */
  DEBUG_LOG(1, "Entered WiFlyDevice::begin()");

  if (!bDifferentUart) SPIuart.begin();
  reboot(); // Reboot to get device into known state
  //requireFlowControl();
  setConfiguration(adhocMode);
}

// TODO: Create a `begin()` that allows IP etc to be supplied.



#define SOFTWARE_REBOOT_RETRY_ATTEMPTS 5

boolean WiFlyDevice::softwareReboot(boolean isAfterBoot = true) {
  /*

   */

  DEBUG_LOG(1, "Entered softwareReboot");

  for (int retryCount = 0;
       retryCount < SOFTWARE_REBOOT_RETRY_ATTEMPTS;
       retryCount++) {

    // TODO: Have the post-boot delay here rather than in enterCommandMode()?

    if (!enterCommandMode(isAfterBoot)) {
      return false; // If the included retries have failed we give up
    }

    uart->println(F("reboot"));

    // For some reason the full "*Reboot*" message doesn't always
    // seem to be received so we look for the later "*READY*" message instead.

    // TODO: Extract information from boot? e.g. version and MAC address

    if (findInResponse("*READY*", 2000)) {
      return true;
    }
  }

  return false;
}

boolean WiFlyDevice::hardwareReboot() {
  /*
   */
  if (!bDifferentUart)
  {
    SPIuart.ioSetDirection(0b00000010);
    SPIuart.ioSetState(0b00000000);
    delay(1);
    SPIuart.ioSetState(0b00000010);
    return findInResponse("*READY*", 2000);
  }
  return softwareReboot();
}


#if USE_HARDWARE_RESET
#define REBOOT hardwareReboot
#else
#define REBOOT softwareReboot
#endif

void WiFlyDevice::reboot() {
  /*
   */

  DEBUG_LOG(1, "Entered reboot");

  if (!REBOOT()) {
    DEBUG_LOG(1, "Failed to reboot. Halting.");
    while (1) {}; // Hang. TODO: Handle differently?
  }
}


boolean WiFlyDevice::sendCommand(const __FlashStringHelper *command,
                                 boolean isMultipartCommand = false,
                                 const char *expectedResponse = "AOK") {
  /*
   */
  DEBUG_LOG(1, "Entered sendCommand");
  DEBUG_LOG(2, "Command:");
  DEBUG_LOG(2, command);
  uart->print(command);
  delay(20);
  if (!isMultipartCommand) {
    uart->flush();
    uart->println();

    // TODO: Handle other responses
    //       (e.g. autoconnect message before it's turned off,
    //        DHCP messages, and/or ERR etc)
    if (!findInResponse(expectedResponse, 1000)) {
      return false;
    }
    //waitForResponse(expectedResponse);
  }
  DEBUG_LOG(2, "sendCommand exit True");

  return true;
}

boolean WiFlyDevice::sendCommand(const char *command,
                                 boolean isMultipartCommand = false,
                                 const char *expectedResponse = "AOK") {
  /*
   */
  DEBUG_LOG(1, "Entered sendCommand");
  DEBUG_LOG(2, "Command:");
  DEBUG_LOG(2, command);
  uart->print(command);
  delay(20);
  if (!isMultipartCommand) {
    uart->flush();
    uart->println();

    // TODO: Handle other responses
    //       (e.g. autoconnect message before it's turned off,
    //        DHCP messages, and/or ERR etc)
    if (!findInResponse(expectedResponse, 1000)) {
      return false;
    }
    //waitForResponse(expectedResponse);
  }
  DEBUG_LOG(2, "sendCommand exit True");

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

  DEBUG_LOG(1, "Entered requireFlowControl");

  enterCommandMode();

  // TODO: Reboot here to ensure we get an accurate response and
  //       don't unintentionally save a configuration we don't intend?

  sendCommand(F("get uart"), false, "Flow=0x");

  while (!uart->available()) {
    // Wait to ensure we have the full response
  }

  char flowControlState = uart->read();

  uart->flush();

  if (flowControlState == '1') {
    return;
  }

  // Enable flow control
  sendCommand(F("set uart flow 1"));

  sendCommand(F("save"), false, "Storing in config");

  // Without this (or some delay--but this seemed more useful/reliable)
  // the reboot will fail because we seem to lose the response from the
  // WiFly and end up with something like:
  //     "*ReboWiFly Ver 2.18"
  // instead of the correct:
  //     "*Reboot*WiFly Ver 2.18"
  // TODO: Solve the underlying problem
  sendCommand(F("get uart"), false, "Flow=0x1");

  reboot();
}

void WiFlyDevice::setConfiguration(boolean adhocMode) {
  /*
   */
  enterCommandMode();

  // TODO: Handle configuration better
  // Turn off auto-connect
  sendCommand(F("set wlan join 0"));

  // TODO: Turn off server functionality until needed
  //       with "set ip protocol <something>"

  // Set server port
  sendCommand(F("set ip localport "), true);
  // TODO: Handle numeric arguments correctly.
  uart->print(serverPort);
  sendCommand("");

  // Turn off remote connect message
  sendCommand(F("set comm remote 0"));

  sendCommand(F("set t z 23"));
  sendCommand(F("set time address 129.6.15.28"));
  sendCommand(F("set time port 123"));
  sendCommand(F("set t e 15"));

  // CDT: Enable the DHCP mode again, if the shield
  // was last used in AdHoc mode we won't do things correctly without
  // these changes.
  if(!adhocMode)
  {
	sendCommand(F("set wlan auth 4"));
	
	sendCommand(F("set ip dhcp 1"));
  } 
  else
  {
	setAdhocParams();
  }
  // Turn off status messages
  // sendCommand(F("set sys printlvl 0"));

  // TODO: Change baud rate and then re-connect?

  // Turn off RX data echo
  // TODO: Should really treat as bitmask
  // sendCommand(F("set uart mode 0"));
}

void WiFlyDevice::setAdhocParams()
{
	// Disable Auto-connect
	sendCommand(F("set wlan join 0"));
	
	// Disable Authentication for AdHoc Mode
	sendCommand(F("set wlan auth 0"));
	
	// Enable Auto IP assignment, This allows the WiFly to automatically
	// assign the IP addresses
	sendCommand(F("set ip d 2"));
}

//
// TODO: Revaluate if this method is actually required. Perhaps the Join method can
// do all of this, and use a internal Private variable to provide all the required parameters
// 
boolean WiFlyDevice::createAdHocNetwork(const char *ssid)
{	
  /*
    Create and AdHoc network with the WiFly Shield.
   */

  DEBUG_LOG(1, "Entered WiFlyDevice::beginAdhoc()");

  reboot(); // Reboot to get device into known state
  
  enterCommandMode();

  // Turn on Adhoc Mode
  sendCommand(F("set wlan join 4"));
  // Set SSID of Adhoc Network
  sendCommand(F("set wlan ssid "),true);
  sendCommand(ssid);
  // Set Channel for Adhoc Network
  sendCommand(F("set wlan chan 1"));
  // Set IP for Adhoc Network
  sendCommand(F("set ip address 169.254.1.1"));
  sendCommand(F("set ip netmask 255.255.0.0"));
  // Turn off DHCP
  sendCommand(F("set ip dhcp 0"));
  // Set server port
  sendCommand(F("set ip localport "), true);
  uart->print(serverPort);
  sendCommand("");

  // Turn off remote connect message
  sendCommand(F("set comm remote 0"));

  sendCommand(F("save"), false, "Storing in config");
  //Ensures sucessful reboot. See requireFlowControl for more info.
  sendCommand(F("get uart"), false, "Flow=0x1");
  reboot();
  
  //After rebooting, your AdHoc network will be available.
}

boolean WiFlyDevice::join(const char *ssid) {
  /*
   */
  // TODO: Handle other authentication methods
  // TODO: Handle escaping spaces/$ in SSID
  // TODO: Allow for timeout?

  // TODO: Do we want to set the passphrase/key to empty when they're
  //       not required? (Probably not necessary as I think module
  //       ignores them when they're not required.)

  sendCommand(F("join "), true);
  // TODO: Actually detect failure to associate
  // TODO: Handle connecting to Adhoc device
  if (sendCommand(ssid, false, "Associated!")) {
    // TODO: Extract information from complete response?
    // TODO: Change this to still work when server mode not active
    waitForResponse("Listen on ");
    skipRemainderOfResponse();
    return true;
  }
  return false;
}


boolean WiFlyDevice::join(const char *ssid, const char *passphrase,
                          boolean isWPA) {
  /*
   */
  // TODO: Handle escaping spaces/$ in passphrase and SSID

  // TODO: Do this better...
  sendCommand(F("set wlan "), true);

  if (isWPA) {
    sendCommand(F("passphrase "), true);
  } else {
    sendCommand(F("key "), true);
  }

  sendCommand(passphrase);

  return join(ssid);
}


#define IP_ADDRESS_BUFFER_SIZE 16 // "255.255.255.255\0"

const char * WiFlyDevice::ip() {
  /*

    The return value is intended to be dropped directly
    into calls to 'print' or 'println' style methods.

   */
  static char ip[IP_ADDRESS_BUFFER_SIZE] = "";

  // TODO: Ensure we're not in a connection?

  enterCommandMode();

  // Version 2.19 of the WiFly firmware has a "get ip a" command but
  // we can't use it because we want to work with 2.18 too.
  sendCommand(F("get ip"), false, "IP=");

  char newChar;
  byte offset = 0;

  // Copy the IP address from the response into our buffer
  while (offset < IP_ADDRESS_BUFFER_SIZE) {
    newChar = uart->read();

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
  while (uart->read() != ' ') {
    // Skip the prompt
  }

  // For some reason the "sendCommand" approach leaves the system
  // in a state where it misses the first/next connection so for
  // now we don't check the response.
  // TODO: Fix this
  uart->println("exit");
  //sendCommand("exit", false, "EXIT");

  return ip;
}

boolean WiFlyDevice::configure(byte option, unsigned long value) {
  /*
   */

  // TODO: Allow options to be supplied earlier?

  switch (option) {
    case WIFLY_BAUD:
      // TODO: Use more of standard command sending method?
      enterCommandMode();
      uart->print("set uart instant ");
      uart->println(value);
      delay(10); // If we don't have this here when we specify the
                 // baud as a number rather than a string it seems to
                 // fail. TODO: Find out why.
      SPIuart.begin(value);
      // For some reason the following check fails if it occurs before
      // the change of SPI UART serial rate above--even though the
      // documentation says the AOK is returned at the old baud
      // rate. TODO: Find out why
      if (!findInResponse("AOK", 100)) {
        return false;
      }
      break;
    default:
      return false;
      break;
  }
  return true;
}


#define TIME_SIZE 11 // 1311006129

long WiFlyDevice::getTime(){

	/*
	Returns the time based on the NTP settings and time zone.
	*/

	char newChar;
	byte offset = 0;
	char buffer[TIME_SIZE+1];

	enterCommandMode();

	//sendCommand("time"); // force update if it's not already updated with NTP server
	sendCommand(F("show t t"), false, "RTC=");

	// copy the time from the response into our buffer
	while (offset < TIME_SIZE) {
			newChar = uart->read();

			if (newChar != -1) {
				buffer[offset++] = newChar;
			}
	}
	buffer[offset]=0;
  // This should skip the remainder of the output.
  // TODO: Handle this better?
  waitForResponse("<");
  findInResponse(" ");

  // For some reason the "sendCommand" approach leaves the system
  // in a state where it misses the first/next connection so for
  // now we don't check the response.
  // TODO: Fix this
  uart->println(F("exit"));
  //sendCommand(F("exit"), false, "EXIT");


  return strtol(buffer, NULL, 0);
}



// Preinstantiate required objects
SpiUartDevice SpiSerial;
WiFlyDevice WiFly(SpiSerial);
