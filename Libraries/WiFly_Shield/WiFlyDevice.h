#ifndef __WIFLY_DEVICE_H__
#define __WIFLY_DEVICE_H__

#include "Configuration.h"

#define DEFAULT_SERVER_PORT 80

class WiFlyDevice {
  public:
    WiFlyDevice(SpiUartDevice& theUart);

    void setUart(Stream* newUart);
    void begin();
    void begin(boolean adhocMode);

    // begin using a static ip (without dhcp)
    // you should provide the ip as a zero terminated string in the
    // usual ip format ("192.168.100.42")
    void beginIP(const char *ip);

	  boolean createAdHocNetwork(const char *ssid);

    boolean join(const char *ssid);
    boolean join(const char *ssid, const char *passphrase, 
                 boolean isWPA = true);
    boolean setWakeSleepTimers( int _wakeTimer, int _sleepTimer);
    void sleepNow();
                 

    boolean configure(byte option, unsigned long value);

	  long getTime();
    const char * getConnectionStatus();
    const char * ip();
    const char * getVersion();
    void useUDP();
    int available();
    char getChar();
    size_t  write(const uint8_t *buffer, size_t size) ;
    void flush();
    boolean readTimeout(char *chp, uint16_t timeout);
    int readBufTimeout(char* buf, int size, uint16_t timeout);
    void flushRx(int timeout);
    
    
    
  private:
    SpiUartDevice& SPIuart;
    Stream* uart;
    boolean bDifferentUart;
    // Okay, this really sucks, but at the moment it works.
    // The problem is that we have to keep track of an active server connection
    // but AFAICT due to the way the WebClient example is written
    // we can't store a useful reference in the server instance
    // to an active client instance because the client object gets copied
    // when it's returned from Server.available(). This means that
    // the state changes in the client object's Client.stop() method
    // never get propagated to the Server's stored active client.
    // Blah, blah, hand-wavy singleton mention. Trying to store the reference
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
    void setConfiguration(boolean adhocMode, const char *ip);
	void setAdhocParams();
    boolean sendCommand(const char *command,
                        boolean isMultipartCommand, // Has default value
                        const char *expectedResponse); // Has default value
    boolean sendCommand(const __FlashStringHelper *command,
                        boolean isMultipartCommand, // Has default value
                        const char *expectedResponse); // Has default value
    void waitForResponse(const char *toMatch);
    void skipRemainderOfResponse();
    boolean responseMatched(const char *toMatch);

    boolean findInResponse(const char *toMatch, unsigned int timeOut);
    boolean enterCommandMode(boolean isAfterBoot = false);
    boolean softwareReboot(boolean isAfterBoot);
    boolean hardwareReboot();
    
    // Methods and variables from WiFly-Shield library from brandenhall
    bool commandModeFlag;
    void exitCommandMode() ;

    friend class WiFlyClient;
    friend class WiFlyServer;
};

#endif
