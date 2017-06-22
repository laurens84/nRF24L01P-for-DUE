// ==========================================================================
//
// File      : RF24.hpp
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
 * @class RF24
 * @author Laurens van der Sluis
 * @date 22/06/2017
 * @file RF24.hpp
 */

#ifndef RF24_H
#define RF24_H

#include "hwlib.hpp"
#include "nRF24L01.h"

/// \brief
/// nRF24L01+ library
/// \details
/// This class implements an interface to an nRF24L01+ module.
///
/// \image html pinout.png
///
/// The nRF24L01+ is a single chip 2.4GHz transceiver with an embedded baseband protocol engine
/// (Enhanced ShockBurst™), suitable for ultra low power wireless applications. The nRF24L01+ is designed
/// for operation in the world wide ISM frequency band at 2.400 - 2.4835GHz.
/// 
/// To design a radio system with the nRF24L01+, you simply need an MCU (microcontroller) and a few external
/// passive components.
/// 
/// You can operate and configure the nRF24L01+ through a Serial Peripheral Interface (SPI). The register
/// map, which is accessible through the SPI, contains all configuration registers in the nRF24L01+ and is
/// accessible in all operation modes of the chip.
/// 
/// The embedded baseband protocol engine (Enhanced ShockBurst™) is based on packet communication
/// and supports various modes from manual operation to advanced autonomous protocol operation. Internal
/// FIFOs ensure a smooth data flow between the radio front end and the system’s MCU. Enhanced Shock-
/// Burst™ reduces system cost by handling all the high speed link layer operations.
/// 
/// The radio front end uses GFSK modulation. It has user configurable parameters like frequency channel,
/// output power and air data rate. nRF24L01+ supports an air data rate of 250 kbps, 1 Mbps and 2Mbps. The
/// high air data rate combined with two power saving modes make the nRF24L01+ very suitable for ultra low
/// power designs.
/// 
/// nRF24L01+ is drop-in compatible with nRF24L01 and on-air compatible with nRF2401A, nRF2402,
/// nRF24E1 and nRF24E2. Intermodulation and wideband blocking values in nRF24L01+ are much
/// improved in comparison to the nRF24L01 and the addition of internal filtering to nRF24L01+ has improved
/// the margins for meeting RF regulatory standards.
/// 
/// Internal voltage regulators ensure a high Power Supply Rejection Ratio (PSRR) and a wide power supply
/// range.
///
class RF24 {
private:
    hwlib::pin_out & CE; /// Chip Enable" pin, activates the RX or TX role.
    hwlib::pin_out & CSN; /// SPI Chip select.
    hwlib::spi_bus & SPI; /// SPI bus.
    uint8_t rx_payload[33]; /// Will hold the data wich has been received.
    uint8_t tx_payload[33]; /// Will hold the data that will be transmitted.
    uint8_t dataout[2]; /// Will hold the SPI command and the dummy byte.
    uint8_t datain[2]; /// Will hold the answer from the RF24 module. First data will always be the status register.
    /// \brief
    /// This will return the rx payload width.
    /// \details
    /// With the DPL feature the nRF24L01+ can decode the payload length of the received packet automatically 
    /// instead of using the RX_PW_Px registers.
    /// The MCU can read the length of the received payload by using this function.
    int read_rx_payload_width();
public:
    RF24(hwlib::pin_out & CE, hwlib::pin_out & CSN, hwlib::spi_bus & SPI);
    
    /// \brief
    /// This will return the state of a bit in a register.
    bool check_bit(const uint8_t reg, const int bit);
    
    /// \brief
    /// This will set a bit in a register to '0'.
    void clear_bit(const uint8_t reg, const int bit);
    
    /// \brief
    /// This will enable dynamic payload on the specified pipe.
    /// \details
    /// In order to enable DPL the EN_DPL bit in the FEATURE register must be enabled. 
    /// In RX mode the DYNPD register must be set. 
    /// A PTX that transmits to a PRX with DPL enabled must have the DPL_P0 bit in DYNPD set.
    void enable_dynamic_payload(const uint8_t pipe);
    
    /// \brief
    /// This will clear the RX FIFO.
    /// \details
    /// Flush RX FIFO, used in RX mode
    /// Should not be executed during transmission of acknowledge, that is, acknowledge package will not be completed.
    void flush_rx();
    
    /// \brief
    /// This will clear the TX FIFO.
    /// \details
    /// Flush TX FIFO, used in TX mode.
    void flush_tx();
    
    /// \brief
    /// This will initialize the chip.
    /// \details
    /// This will set the CE and CSN pins in their default states, and then check if the chip is detected.
    /// The default state for CE is '0'.
    /// The default state for CSN is '1'.
    /// This function will try to read the status register. If the chip respons as expected, it will return '1'.
    /// If the chip does not respond as expected, it will return '0'.
    bool init();
    
    /// \brief
    /// This will return the configuration of the specified register.
    uint8_t read_register(const uint8_t reg);
    
    /// \brief
    /// This will return the data that has been received on the chip.
    /// \details
    /// This function will  first check if the chip as received any data.
    /// If there is data ready for us, it will read it and return an array.
    /// The first byte in this array will hold the length of the array.
    /// So if you received seven bytes, the array will be eight bytes long, and the value of the first byte will be '8'.
    uint8_t* receive();
    
    /// \brief
    /// This will send the specified data.
    /// \details
    /// This function will first check if you are not sending more than 32 bytes (32 bytes is max).
    /// It will then set the chip in TX mode, if it's not already.
    /// Then it will load the specified data in the chip and send it.
    /// If sending succeeded this function will return '1'.
    /// if sending failed it wil return '0'.
    /// in both senarios, the chip will be set in the same state, (RX mode or TX mode) as it was when this function was called.
    bool send(const uint8_t* tx_payload, const int bytes);
    
    /// \brief
    /// This will set a bit in a register to '1'.
    void set_bit(const uint8_t reg, const int bit);
    
    /// \brief
    /// Sets the frequency channel the chip operates on.
    /// \details
    /// This function will set the channel of the chip.
    /// The register allows you to set the channel from 0 to 127, and each channel has a separation of 1 MHz.
    /// Watch out for the ISM band guidelines of the country in which you are operating your 24L01!!!
    /// The FCC in the US only allows you to go from 2.4 to 2.4835 GHz in the ISM band, 
    /// which gives you 835 MHz or the first 84 channels to use.
    void set_channel(const int channel);
    
    /// \brief
    /// Sets the number of bytes the payload will be.
    /// \details
    /// If Dynamic Payload is not used, the payload width will have to be specified.
    /// This function sets the payload width.
    /// Max is 32 bytes.
    /// If set to zero, pipe will not be used.
    void set_payload_width(const uint8_t pipe, const int bytes);
    
    /// \brief
    /// Sets the RX address of the specified pipe.
    /// \details
    /// This function will set the address of the specified pipe.
    /// Pipe0 and pipe1 both have 5 bytes.
    /// Pipe 2, 3, 4 and 5 have the first same four bytes as pipe0.
    /// Only the last byte of those is unique.
    void set_rx_address(const uint8_t pipe, const uint8_t* address);
    
    /// \brief
    /// Sets the TX address.
    /// \details
    /// Transmit address. Used for a PTX device only.
    /// Set RX_ADDR_P0 equal to this address to handle automatic acknowledge if this is a PTX device with
    /// Enhanced ShockBurst™ enabled.
    void set_tx_address(const uint8_t* address);
    
    /// \brief
    /// Configure and start the nRF24L01+ module the easy way.
    /// \details
    /// This will configure and start the chip for immediate easy use.
    /// The chip will use one data pipe on address: {0x26, 0x02, 0x19, 0x96, 0xDD}
    /// This is also the address it will send on.
    /// It will operate on channel 24.
    /// Auto acknowledge will be enabled.
    /// Dynamic payload will be enabled.
    /// You will be able to send and receive payloads of 1-32 bytes.
    /// The chip will always be listening unless it is told to send something.
    /// When sending is done, the chip will be listening again.
    void start_easy_mode();
    
    /// \brief
    /// This function will set the specified register, to the specified setting.
    void write_register(const uint8_t reg, const uint8_t value);
};

#endif