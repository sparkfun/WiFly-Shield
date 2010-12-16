
// We use the name "_Spi.h" to avoid a clash with the other SPI library which
// the IDE doesn't notice the difference between.

#ifndef ___SPI_H__
#define ___SPI_H__

#include <WProgram.h>

#include <pins_arduino.h>


// TODO: Do we want to use this instead: <http://www.arduino.cc/playground/Code/Spi>
class SpiDevice {
  public:
    SpiDevice();
    
    void begin();
    void begin(byte selectPin);

    // TODO: Make these private (or protected) in the final library?
    void deselect();
    void select();

    byte transfer(volatile byte data);
    void transfer_bulk(const uint8_t* srcptr, unsigned long int length);

    
  private:
    void _initPins();
    void _initSpi();    
  
    byte _selectPin;
    
};

#endif

