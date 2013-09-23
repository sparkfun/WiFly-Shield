
// Based on interface defined in Ethernet's Client.h

#ifndef __WIFLY_CLIENT_H__
#define __WIFLY_CLIENT_H__

#include "Stream.h"

#include "ParsedStream.h"

#include "WiFlyDevice.h"

class WiFlyClient : public Stream {
 public:
  WiFlyClient(uint8_t *ip, uint16_t port);
  WiFlyClient(const char* domain, uint16_t port);

  boolean connect();

#if ARDUINO >= 100
  size_t write(byte value);
  size_t write(const char *str);
  size_t write(const uint8_t *buffer, size_t size);
#else
  void write(byte value);
  void write(const char *str);
  void write(const uint8_t *buffer, size_t size);
#endif

  int available();
  int read();
  char readChar();
  void flush(void);
  int peek();

  bool connected();
  void stop();

  operator bool();


  uint8_t *_ip;
  uint16_t _port;

  const char *_domain;
private:
  WiFlyDevice& _WiFly;


  bool isOpen;

  ParsedStream stream;


  // TODO: Work out why alternate instantiation code in
  //       Server.available() doesn't work and thus requires this:
  friend class Server;
};

#endif
