#include "RF24.hpp"

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

int RF24::read_rx_payload_width(){
    dataout[0] = R_RX_PL_WID;
    dataout[1] = NOP;
    SPI.write_and_read( CSN, 2, &*dataout, &*datain );
    return datain[1];
}

uint8_t* RF24::receive(){
    CE.set(0);
    const int bytes = this->read_rx_payload_width() + 1;
    payload[0] = R_RX_PAYLOAD;
    SPI.write_and_read( CSN, bytes, &*payload, &*payload );
    payload[0] = bytes;
    this->write_register(NRF_STATUS, (1<<(RX_DR - 1)));
    return payload;
}

bool RF24::send(const uint8_t* tx_payload, const int bytes){
    CE.set(0);
    for (int i = 0; i < bytes; i++){
        payload[i + 1] = tx_payload[i];
    }
    payload[0] = W_TX_PAYLOAD;
    SPI.write_and_read( CSN, (bytes + 1), &*payload, nullptr );
    CE.set(1);
    hwlib::wait_us(10);
    CE.set(0);
    if ((this->read_register(NRF_STATUS)[1] << (TX_DS - 1)) & 1){
        this->write_register(NRF_STATUS, (1 << (TX_DS - 1)));
        return true;
    }
    else
        return false;
}