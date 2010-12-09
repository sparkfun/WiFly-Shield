
// Based on ring buffer implementation in `HardwareSerial`.
// TODO: Do proper license stuff

#ifndef __PARSED_STREAM_H__
#define __PARSED_STREAM_H__

#define RX_BUFFER_SIZE 64

#include <string.h>

#include "SpiUart.h"

struct ring_buffer {
  unsigned char buffer[RX_BUFFER_SIZE];
  int head;
  int tail;
};

const static char *MATCH_TOKEN = "*CLOS*";

class ParsedStream {
private:  
  ring_buffer _rx_buffer;

  void getByte();
  void storeByte(unsigned char c);

  unsigned int bytes_matched;

  uint8_t available(bool raw);

  int freeSpace();

  bool _closed;

  SpiUartDevice& _uart;

public:
  ParsedStream(SpiUartDevice& theUart);
  uint8_t available(void);
  int read(void);

  bool closed();

  void reset();
};


#endif
