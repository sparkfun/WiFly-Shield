
// We use the name "_Spi.h" to avoid a clash with the other SPI library which
// the IDE doesn't notice the difference between.

#ifndef ___SPI_H__
#define ___SPI_H__

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <pins_arduino.h>


// TODO: Do we want to use this instead: <http://www.arduino.cc/playground/Code/Spi>
class SpiDevice {
  public:
    SpiDevice();

    byte transfer(volatile byte data);
};

#endif

