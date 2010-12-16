#include "_Spi.h"

SpiDevice::SpiDevice() {
  /*
   */

}


void SpiDevice::begin() {
  /*
    
    Use the default chip select pin as specified by
    the SS define in the header.
    
   */
  begin(SS);
}


void SpiDevice::begin(byte selectPin) {
  /*
   */
  _selectPin = selectPin;      
  _initPins();
  _initSpi();
}


void SpiDevice::_initPins() {
  /*
  
    Initialise the pins used for SPI communication.
  
   */
  
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  pinMode(SCK, OUTPUT);
  pinMode(_selectPin, OUTPUT);
  
  deselect();
}


void SpiDevice::_initSpi() {
  /*
  
    Initialise SPI system.
  
   */

  SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0);

#if 0
  // This approach is not specified in the datasheet so I'm trying without.
  char clr = 0;
  
  clr=SPSR;
  clr=SPDR;
  delay(10); 
#endif
  
}


void SpiDevice::deselect() {
  /*
  
    Deselect the SPI device.
    
  */
  digitalWrite(_selectPin, HIGH);
}


void SpiDevice::select() {
  /*
  
    Select the SPI device.
    
  */
  digitalWrite(_selectPin, LOW);
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

