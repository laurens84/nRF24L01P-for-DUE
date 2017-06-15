#ifndef RF24_H
#define RF24_H

#include "hwlib.hpp"
#include "nRF24L01.h"

class RF24 {
private:
    hwlib::pin_out & CE, & CSN;
    hwlib::spi_bus & SPI;
    uint8_t payload[33];
    uint8_t dataout[2];
    uint8_t datain[2];
public:
    RF24(hwlib::pin_out & CE, hwlib::pin_out & CSN, hwlib::spi_bus & SPI);
    uint8_t* read_register(const uint8_t reg);
    void write_register(const uint8_t reg, const uint8_t value);
    int read_rx_payload_width();
    uint8_t* receive();
    bool send(const uint8_t* tx_payload, const int bytes);
    void flush_tx();
    void flush_rx();
    void set_channel(int channel);
    bool check_bit(uint8_t reg, int bit);
    void set_bit(uint8_t reg, int bit);
    void clear_bit(uint8_t reg, int bit);
};

#endif