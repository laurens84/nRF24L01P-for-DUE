#include "hwlib.hpp"
#include "RF24.hpp"

int main( void ){
    // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;
    
    auto CE1 = hwlib::target::pin_out( hwlib::target::pins::d8 );
    auto CSN1 = hwlib::target::pin_out( hwlib::target::pins::d9 );
    auto SCLK1 = hwlib::target::pin_out( hwlib::target::pins::d10 );
    auto MOSI1 = hwlib::target::pin_out( hwlib::target::pins::d11 );
    auto MISO1 = hwlib::target::pin_in( hwlib::target::pins::d12 );
    
    auto CE2 = hwlib::target::pin_out( hwlib::target::pins::d3 );
    auto CSN2 = hwlib::target::pin_out( hwlib::target::pins::d2 );
    auto SCLK2 = hwlib::target::pin_out( hwlib::target::pins::d5 );
    auto MOSI2 = hwlib::target::pin_out( hwlib::target::pins::d4 );
    auto MISO2 = hwlib::target::pin_in( hwlib::target::pins::d6 );
    
    auto SPI1 = hwlib::spi_bus_bit_banged_sclk_mosi_miso( SCLK1, MOSI1, MISO1 );
    
    auto SPI2 = hwlib::spi_bus_bit_banged_sclk_mosi_miso( SCLK2, MOSI2, MISO2 );
    
    hwlib::wait_ms(500);
    
    uint8_t dataout[32] = {0x11, 0x32, 0x8C, 0xEA, 0xE1, 0xF2, 0x6D, 0x39,
                           0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
                           0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21,
                           0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29};
    
    RF24 transmitter(CE1, CSN1, SPI1);
    RF24 receiver(CE2, CSN2, SPI2);
    
    if (!transmitter.init()) return 1;
    if (!receiver.init()) return 1;
    
    transmitter.start_easy_mode();
    receiver.start_easy_mode();
        
    hwlib::cout << "Send succesfull: " << transmitter.send(&*dataout, 32) << '\n';
    hwlib::wait_ms(2000);
    hwlib::cout << "RX Status: " << hwlib::bin << unsigned(receiver.read_register(NRF_STATUS)) << '\n';
    
    if (receiver.check_bit(NRF_STATUS, RX_DR)){
        uint8_t* datain = receiver.receive();
        for (int i = 1; i < datain[0]; i++){
            hwlib::cout << "Data[" << hwlib::dec << i << "] " << hwlib::hex << unsigned(datain[i]) << '\n';
        }
        hwlib::cout << "RX Status: " << hwlib::bin << unsigned(receiver.read_register(NRF_STATUS)) << '\n';
    }
    return 0;
}