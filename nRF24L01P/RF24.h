#ifndef RF24_H
#define RF24_H

#include "hwlib.hpp"
#include "nRF24L01.h"

class RF24 {
private:
    hwlib::pin_out & CE, & CSN;
    hwlib::spi_bus & SPI;
    uint8_t dataout[2];
    uint8_t datain[2];
public:
    RF24(hwlib::pin_out & CE, hwlib::pin_out & CSN, hwlib::spi_bus & SPI);
    uint8_t* read_register(const uint8_t reg);
    void write_register(const uint8_t reg, const uint8_t value);
};

#endif