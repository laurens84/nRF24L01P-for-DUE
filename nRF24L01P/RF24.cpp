#include "RF24.h"

RF24::RF24(hwlib::pin_out & CE, hwlib::pin_out & CSN, hwlib::spi_bus & SPI):
    CE( CE ),
    CSN( CSN ),
    SPI( SPI )
{}
    
uint8_t* RF24::read_register(const uint8_t reg){
    dataout[0] = (R_REGISTER | reg);
    dataout[1] = NOP;
    SPI.write_and_read( CSN, 2, &*dataout, &*datain );
    return datain;
}

void RF24::write_register(const uint8_t reg, const uint8_t value){
    dataout[0] = (W_REGISTER | reg);
    dataout[1] = value;
    SPI.write_and_read( CSN, 2, &*dataout, nullptr );
    return;
}