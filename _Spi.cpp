#include "_Spi.h"

SpiDevice::SpiDevice() {
  /*
   */

}


void SpiDevice::deselect() {
  /*
  
    Deselect the SPI device.
    
  */
  digitalWrite(SS, HIGH);
}


void SpiDevice::select() {
  /*
  
    Select the SPI device.
    
  */
  digitalWrite(SS, LOW);
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


void SpiDevice::transfer_bulk(const uint8_t* srcptr, unsigned long int length) {
  /*

    Transfer an array of bytes to the SPI device.
    
    Discards all returned bytes.
  
   */
  for(unsigned long int offset = 0; offset < length; offset++) {
    transfer(srcptr[offset]);
  }
}

