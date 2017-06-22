// ==========================================================================
//
// File      : RF24.cpp
// Part of   : C++ library for using the nRF24L01+ module.
// Copyright : laurens@vandersluisonline.com 2017
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at 
// http://www.boost.org/LICENSE_1_0.txt
//
// ==========================================================================

// this file contains Doxygen lines
/**
 * @author Laurens van der Sluis
 * @date 22/06/2017
 * @file RF24.cpp
 */

#include "RF24.hpp"

RF24::RF24(hwlib::pin_out & CE, hwlib::pin_out & CSN, hwlib::spi_bus & SPI):
    CE( CE ),
    CSN( CSN ),
    SPI( SPI )
{}

bool RF24::check_bit(const uint8_t reg, const int bit){
    uint8_t regsetting = this->read_register(reg); // Get register setting.
    bool bitstate = (regsetting >> bit) & 1; // Check the bit-state.
    return bitstate;
}

void RF24::clear_bit(const uint8_t reg, const int bit){
    uint8_t regsetting = 0; // Will hold a bit pattern.
    // To clear a bit in the status register, we have to send a '1'.
    // For other registers we have to send a '0'.
    if (reg == NRF_STATUS){
        regsetting |= (1 << bit); // Set the bit.
    }
    else {
        regsetting = this->read_register(reg); // Get the actual register setting.
        regsetting &= ~(1 << bit); // Clear the bit.
    }
    this->write_register(reg, regsetting); // Write the bit pattern.
    return;
}

void RF24::enable_dynamic_payload(const uint8_t pipe){
    this->set_bit(FEATURE, EN_DPL); // Enable dynamic payload.
    this->set_bit(DYNPD, pipe); // Enable dynamic payload for this pipe.
    return;
}

void RF24::flush_rx(){
    dataout[0] = FLUSH_RX; // Place command in first byte.
    SPI.write_and_read(CSN, 1, &*dataout, nullptr); // Send command.
    return;
}

void RF24::flush_tx(){
    dataout[0] = FLUSH_TX; // Place command in first byte.
    SPI.write_and_read(CSN, 1, &*dataout, nullptr); // Send command.
    return;
}

bool RF24::init(){
    CSN.set(1); // SPI Chip Select is active low, so we set it high now.
    CE.set(0); // Chip Enable.
    hwlib::wait_ms(10); // Let the chip settle.
    // Test the chip. If a chip just powered up, the status register will be 1110.
    if (this->read_register(NRF_STATUS) == 0xE) return true;
    else return false;
}

uint8_t* RF24::receive(){
    // If there is data ready for us, read it.
    if (this->check_bit(NRF_STATUS, RX_DR)){
        CE.set(0); // Stop listening for incomming data.
        const int bytes = this->read_rx_payload_width() + 1; // Check the number of bytes we have.
        tx_payload[0] = R_RX_PAYLOAD; // Set the reading command.
        SPI.write_and_read( CSN, bytes, &*tx_payload, &*rx_payload ); // Get our data.
        rx_payload[0] = bytes; // Set the first element of our array to the amount of bytes we have.
        this->clear_bit(NRF_STATUS, RX_DR); // Clear the status register.
        CE.set(1);
    }
    // There is no data.
    else{
        rx_payload[0] = 0;
    }
    return rx_payload;
}

uint8_t RF24::read_register(const uint8_t reg){
    dataout[0] = (R_REGISTER | reg); // Create our command and set it.
    dataout[1] = NOP; // Dummy byte.
    SPI.write_and_read( CSN, 2, &*dataout, &*datain ); // Send command and get our answer.
    return datain[1]; // Our asnwer is in the second byte.
}

int RF24::read_rx_payload_width(){
    dataout[0] = R_RX_PL_WID; // Set the command.
    dataout[1] = NOP; // Dummy byte.
    SPI.write_and_read( CSN, 2, &*dataout, &*datain ); // Send command and get our answer.
    return datain[1]; // Our answer is in the second byte.
}

bool RF24::send(const uint8_t* data, const int bytes){
    if (bytes > 32) return false; // We can only send up to 32 bytes, return false if it's more.
    CE.set(0); // Make sure the chip is idle.
    bool RX_mode = this->check_bit(NRF_CONFIG, PRIM_RX); // Save the role of the chip.
    if (RX_mode) this->clear_bit(NRF_CONFIG, PRIM_RX); // If the chip is in RX mode, set it to TX.
    this->flush_tx(); // Flush tx register so we are sure there is no junk in it.
    // Copy the data in our payload array.
    for (int i = 0; i < bytes; i++){
        tx_payload[i + 1] = data[i];
    }
    tx_payload[0] = W_TX_PAYLOAD; // Set our command at the first index.
    SPI.write_and_read( CSN, (bytes + 1), &*tx_payload, nullptr ); // Send the payload to the chip.
    CE.set(1);  // Broadcast the payload.
    hwlib::wait_us(10); // Give the chip some time to send.
    CE.set(0); // Stop the broadcast.
    hwlib::wait_ms(1); // Wait for the chip to settle.
    // Check if sending was succesfull.
    if (this->check_bit(NRF_STATUS, TX_DS)){
        this->clear_bit(NRF_STATUS, TX_DS);
        // If the chip was in RX mode, set it back. Else, leave it in TX mode.
        if (RX_mode) {
            this->set_bit(NRF_CONFIG, PRIM_RX);
            CE.set(1);
        }
        return true;
    }
    else {
        // If the chip was in RX mode, set it back. Else, leave it in TX mode.
        if (RX_mode) {
            this->set_bit(NRF_CONFIG, PRIM_RX);
            CE.set(1);
        }
        return false;
    }
}

void RF24::set_bit(const uint8_t reg, const int bit){
    uint8_t regsetting = this->read_register(reg); // Get actual register setting.
    regsetting |= (1 << bit); // Set the bit we want to change.
    this->write_register(reg, regsetting); // Write back the new register setting.
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
    uint8_t data[6]; // Will hold our command byte along with the address.
    data[0] = (W_REGISTER | pipe); // Create and set the command byte.
    
    // Check witch pipe address we are changing.
    if ((pipe == RX_ADDR_P0) || pipe == RX_ADDR_P1){
        // Place our new address in our array.
        for (int i = 0; i < 5; i++){
            data[i + 1] = address[i];
        }
        SPI.write_and_read( CSN, 6, &*data, nullptr ); // Sent the new address to the chip.
    }
    else {
        // Pipe 2, 3, 4 and 5 only have one changable address byte.
        data[1] = address[0]; // Set the new address byte in our array.
        SPI.write_and_read( CSN, 2, &*data, nullptr ); // Sent the new address to the chip.
    }
    return;
}

void RF24::set_tx_address(const uint8_t* address){
    uint8_t data[6]; // Will hold our command byte along with the address.
    data[0] = (W_REGISTER | TX_ADDR); // Create and set our command.
    // Place our new address in our array.
    for (int i = 0; i < 5; i++){
        data[i + 1] = address[i];
    }
    SPI.write_and_read( CSN, 6, &*data, nullptr ); // Sent the new address to the chip.
    return;
}

void RF24::start_easy_mode(){
    uint8_t address[5] = {0x26, 0x02, 0x19, 0x96, 0xDD}; // Create the address we are going to use.
    this->set_channel(42); // Set the channel number.
    this->set_rx_address(RX_ADDR_P0, &*address); // Set the RX address of pipe 0.
    this->set_tx_address(&*address); // Set the TX address.
    this->enable_dynamic_payload(DPL_P0); // Enable dynamic payload.
    this->set_bit(NRF_CONFIG, PRIM_RX); // Set the chip to RX mode.
    this->set_bit(NRF_CONFIG, PWR_UP); // Power up the chip.
    CE.set(1); // Activate the chip.
}

void RF24::write_register(const uint8_t reg, const uint8_t value){
    dataout[0] = (W_REGISTER | reg); // Create our command and set it.
    dataout[1] = value; // Hold the new register setting.
    SPI.write_and_read( CSN, 2, &*dataout, nullptr ); // Send our new register setting to the chip.
    return;
}