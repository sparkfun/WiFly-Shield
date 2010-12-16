#ifndef __WIFLY_DEVICE_H__
#define __WIFLY_DEVICE_H__

#include "Configuration.h"

#define DEFAULT_SERVER_PORT 80

class WiFlyDevice {
  public:
    WiFlyDevice(SpiUartDevice& theUart);
    void begin();

    boolean join(const char *ssid);
    boolean join(const char *ssid, const char *passphrase, 
		 boolean isWPA = true);

    const char * ip();
    
  private:
    SpiUartDevice& uart;

    // Okay, this really sucks, but at the moment it works.
    // The problem is that we have to keep track of an active server connection
    // but AFAICT due to the way the WebClient example is written
    // we can't store a useful reference in the server instance
    // to an active client instance because the client object gets copied
    // when it's returned from Server.available(). This means that
    // the state changes in the client object's Client.stop() method
    // never get propagated to the Server's stored active client.
    // Blah, blah, handwavy singleton mention. Trying to store the reference
    // to the active client connection here runs into apparent circular
    // reference issues with header includes. So in an effort to get this out
    // the door we just share whether or not the current "active client"
    // that the server has a stored reference is actually active or not.
    // (Yeah, nice.)
    // TODO: Handle this better.
    boolean serverConnectionActive;

    uint16_t serverPort;      
    
    // TODO: Should these be part of a different class?
    // TODO: Should all methods that need to be in command mode ensure
    //       they are first?
    void attemptSwitchToCommandMode();
    void switchToCommandMode();
    void reboot();
    void requireFlowControl();
    void setConfiguration();
    boolean sendCommand(const char *command,
                        boolean isMultipartCommand, // Has default value
                        const char *expectedResponse); // Has default value
    void waitForResponse(const char *toMatch);
    void skipRemainderOfResponse();
    boolean responseMatched(const char *toMatch);

    boolean findInResponse(const char *toMatch, unsigned int timeOut);
    boolean enterCommandMode(boolean isAfterBoot = false);
    boolean softwareReboot(boolean isAfterBoot);
    boolean hardwareReboot();

    friend class Client;
    friend class Server;
};

#endif
