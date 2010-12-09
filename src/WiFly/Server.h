#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdint.h>
 
class Server { // TODO: Should subclass Print to be consistent
 public:
  Server (uint16_t port);
  Client& available();
  
  void begin();

 private:
  uint16_t _port;  
  Client activeClient;
};

#endif
