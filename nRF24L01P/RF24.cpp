#include "RF24.hpp"

RF24::RF24(hwlib::pin_out & CE, hwlib::pin_out & CSN, hwlib::spi_bus & SPI):
    CE( CE ),
    CSN( CSN ),
    SPI( SPI )
{}

bool RF24::check_bit(const uint8_t reg, const int bit){
    uint8_t regsetting = this->read_register(reg);
    bool bitstate = (regsetting >> bit) & 1;
    return bitstate;
}

void RF24::clear_bit(const uint8_t reg, const int bit){
    uint8_t regsetting = 0;
    if (reg == NRF_STATUS){
        regsetting |= (1 << bit);
    }
    else {
        regsetting = this->read_register(reg);
        regsetting &= ~(1 << bit);
    }
    this->write_register(reg, regsetting);
    return;
}

void RF24::enable_dynamic_payload(const uint8_t pipe){
    this->set_bit(FEATURE, EN_DPL);
    this->set_bit(DYNPD, pipe);
    return;
}

void RF24::flush_rx(){
    dataout[0] = FLUSH_RX;
    SPI.write_and_read(CSN, 1, &*dataout, nullptr);
    return;
}

void RF24::flush_tx(){
    dataout[0] = FLUSH_TX;
    SPI.write_and_read(CSN, 1, &*dataout, nullptr);
    return;
}

bool RF24::init(){
    CSN.set(1);
    CE.set(0);
    hwlib::wait_ms(10);
    if (this->read_register(NRF_STATUS) == 0xE) return true;
    else return false;
}

uint8_t* RF24::receive(){
    if (this->check_bit(NRF_STATUS, RX_DR)){
        CE.set(0);
        const int bytes = this->read_rx_payload_width() + 1;
        tx_payload[0] = R_RX_PAYLOAD;
        SPI.write_and_read( CSN, bytes, &*tx_payload, &*rx_payload );
        rx_payload[0] = bytes;
        this->clear_bit(NRF_STATUS, RX_DR);
        CE.set(1);
    }
    else{
        rx_payload[0] = 0;
    }
    return rx_payload;
}

uint8_t RF24::read_register(const uint8_t reg){
    dataout[0] = (R_REGISTER | reg);
    dataout[1] = NOP;
    SPI.write_and_read( CSN, 2, &*dataout, &*datain );
    return datain[1];
}

int RF24::read_rx_payload_width(){
    dataout[0] = R_RX_PL_WID;
    dataout[1] = NOP;
    SPI.write_and_read( CSN, 2, &*dataout, &*datain );
    return datain[1];
}

bool RF24::send(const uint8_t* data, const int bytes){
    if (bytes > 32) return false;
    CE.set(0);
    bool RX_mode = this->check_bit(NRF_CONFIG, PRIM_RX);
    if (RX_mode) this->clear_bit(NRF_CONFIG, PRIM_RX);
    this->flush_tx();
    for (int i = 0; i < bytes; i++){
        tx_payload[i + 1] = data[i];
    }
    tx_payload[0] = W_TX_PAYLOAD;
    SPI.write_and_read( CSN, (bytes + 1), &*tx_payload, nullptr );
    CE.set(1);
    hwlib::wait_us(10);
    CE.set(0);
    hwlib::wait_ms(1);
    if (this->check_bit(NRF_STATUS, TX_DS)){
        this->clear_bit(NRF_STATUS, TX_DS);
        if (RX_mode) {
            this->set_bit(NRF_CONFIG, PRIM_RX);
            CE.set(1);
        }
        return true;
    }
    else {
        if (RX_mode) {
            this->set_bit(NRF_CONFIG, PRIM_RX);
            CE.set(1);
        }
        return false;
    }
}

void RF24::set_bit(const uint8_t reg, const int bit){
    uint8_t regsetting = this->read_register(reg);
    regsetting |= (1 << bit);
    this->write_register(reg, regsetting);
    return;
}

void RF24::set_channel(const int channel){
    this->write_register(RF_CH, channel);
    return;
}

void RF24::set_payload_width(const uint8_t pipe, const int bytes){
    this->write_register(pipe, bytes);
}

void RF24::set_rx_address(const uint8_t pipe, const uint8_t* address){
    uint8_t data[6];
    data[0] = (W_REGISTER | pipe);
    if ((pipe == RX_ADDR_P0) || pipe == RX_ADDR_P1){
        for (int i = 0; i < 5; i++){
            data[i + 1] = address[i];
        }
        SPI.write_and_read( CSN, 6, &*data, nullptr );
    }
    else {
        data[1] = address[0];
        SPI.write_and_read( CSN, 2, &*data, nullptr );
    }
    return;
}

void RF24::set_tx_address(const uint8_t* address){
    uint8_t data[6];
    data[0] = (W_REGISTER | TX_ADDR);
    for (int i = 0; i < 5; i++){
        data[i + 1] = address[i];
    }
    SPI.write_and_read( CSN, 6, &*data, nullptr );
    return;
}

void RF24::start_easy_mode(){
    uint8_t address[5] = {0x26, 0x02, 0x19, 0x96, 0xDD};
    this->set_channel(42);
    this->set_rx_address(RX_ADDR_P0, &*address);
    this->set_tx_address(&*address);
    this->enable_dynamic_payload(DPL_P0);
    this->set_bit(NRF_CONFIG, PRIM_RX);
    this->set_bit(NRF_CONFIG, PWR_UP);
    CE.set(1);
}

void RF24::write_register(const uint8_t reg, const uint8_t value){
    dataout[0] = (W_REGISTER | reg);
    dataout[1] = value;
    SPI.write_and_read( CSN, 2, &*dataout, nullptr );
    return;
}