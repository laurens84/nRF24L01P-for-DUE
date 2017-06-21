#ifndef RF24_H
#define RF24_H

#include "hwlib.hpp"
#include "nRF24L01.h"

class RF24 {
private:
    hwlib::pin_out & CE, & CSN;
    hwlib::spi_bus & SPI;
    uint8_t rx_payload[33];
    uint8_t tx_payload[33];
    uint8_t dataout[2];
    uint8_t datain[2];
    int read_rx_payload_width();
public:
    RF24(hwlib::pin_out & CE, hwlib::pin_out & CSN, hwlib::spi_bus & SPI);
    bool check_bit(const uint8_t reg, const int bit);
    void clear_bit(const uint8_t reg, const int bit);
    void enable_dynamic_payload(const uint8_t pipe);
    void flush_rx();
    void flush_tx();
    bool init();
    uint8_t read_register(const uint8_t reg);
    uint8_t* receive();
    bool send(const uint8_t* tx_payload, const int bytes);
    void set_bit(const uint8_t reg, const int bit);
    void set_channel(const int channel);
    void set_payload_width(const uint8_t pipe, const int bytes);
    void set_rx_address(const uint8_t pipe, const uint8_t* address);
    void set_tx_address(const uint8_t* address);
    void start_easy_mode();
    void write_register(const uint8_t reg, const uint8_t value);
};

#endif