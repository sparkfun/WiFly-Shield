
#ifndef __SPIUART_H__
#define __SPIUART_H__

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <SPI.h>
#include <pins_arduino.h>

#include "Configuration.h"

// SC16IS750 Register definitions
// TODO: Don't bit shift these here, do it in the read/write register routines
#define THR        0x00 << 3
#define RHR        0x00 << 3
#define IER        0x01 << 3
#define FCR        0x02 << 3
#define IIR        0x02 << 3
#define LCR        0x03 << 3
#define MCR        0x04 << 3
#define LSR        0x05 << 3
#define MSR        0x06 << 3
#define SPR        0x07 << 3
#define TXLVL      0x08 << 3
#define RXLVL      0x09 << 3
#define DLAB       0x80 << 3
#define IODIR      0x0A << 3
#define IOSTATE    0x0B << 3
#define IOINTMSK   0x0C << 3
#define IOCTRL     0x0E << 3
#define EFCR       0x0F << 3

#define DLL        0x00 << 3
#define DLM        0x01 << 3
#define EFR        0x02 << 3
#define XON1       0x04 << 3
#define XON2       0x05 << 3
#define XOFF1      0x06 << 3
#define XOFF2      0x07 << 3


// See Chapter 11 of datasheet
#define SPI_READ_MODE_FLAG 0x80

#define BAUD_RATE_DEFAULT 9600 // WiFly default baudrate


class SpiUartDevice : public Stream {
  
  public:
    void begin(unsigned long baudrate = BAUD_RATE_DEFAULT);
    int available();
    int read();
    size_t write(byte value);
    size_t write(const char *str, size_t size);
    using Print::write;

    void flush();

//required for Stream
    int peek() {return 0;};

    // These are specific to the SPI UART
    void ioSetDirection(unsigned char bits);
    void ioSetState(unsigned char bits);
  
  private:
    void deselect();
    void select();
    void writeRegister(byte registerAddress, byte data);
    byte readRegister(byte registerAddress);
    void initUart(unsigned long baudrate);
    void configureUart(unsigned long baudrate);  
    void setBaudRate(unsigned long baudrate);
    boolean uartConnected();
};

#endif 

