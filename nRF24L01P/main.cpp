#include "hwlib.hpp"
#include <stdint.h>
#include "nRF24L01.h"
#include "RF24.h"

int main( void ){
    // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;

    auto CE = hwlib::target::pin_out( hwlib::target::pins::d8 );
    auto CSN = hwlib::target::pin_out( hwlib::target::pins::d9 );
    auto SCLK = hwlib::target::pin_out( hwlib::target::pins::d10 );
    auto MOSI = hwlib::target::pin_out( hwlib::target::pins::d11 );
    auto MISO = hwlib::target::pin_in( hwlib::target::pins::d12 );
    CSN.set(1);
    
    auto SPI = hwlib::spi_bus_bit_banged_sclk_mosi_miso( SCLK, MOSI, MISO );
    
    hwlib::wait_ms(500);
    
    RF24 test(CE, CSN, SPI);
    
    uint8_t *x;
    
    x = test.read_register(RF_CH);
    hwlib::cout << hwlib::bin << unsigned(x[0]) << '\n' << hwlib::bin << unsigned(x[1]) << '\n';
    
    test.write_register(RF_CH, 2);
    
    hwlib::wait_ms(5000);
    
    x = test.read_register(RF_CH);
    hwlib::cout << hwlib::bin << unsigned(x[0]) << '\n' << hwlib::bin << unsigned(x[1]) << '\n';
    
    return 0;
}