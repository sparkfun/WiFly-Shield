#include "_Spi.h"

SpiDevice::SpiDevice() {
  /*
   */

}





byte SpiDevice::transfer(volatile byte data) {
  /*

    Transfer byte to SPI device and return
    the byte retrieved.

   */  
  SPDR = data; // Start the transmission
  while (!(SPSR & (1<<SPIF))) {
    // Wait for the end of the transmission
  };
  return SPDR;  // Return the received byte
}

