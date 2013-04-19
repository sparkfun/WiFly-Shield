#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdint.h>
 
class WiFlyServer { // TODO: Should subclass Print to be consistent
 public:
  WiFlyServer (uint16_t port);
  WiFlyClient& available();
  
  void begin();

 private:
  uint16_t _port;  
  WiFlyClient activeClient;
};

#endif
