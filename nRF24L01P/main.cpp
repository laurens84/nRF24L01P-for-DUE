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
    CSN1.set(1);
    
    auto CE2 = hwlib::target::pin_out( hwlib::target::pins::d3 );
    auto CSN2 = hwlib::target::pin_out( hwlib::target::pins::d2 );
    auto SCLK2 = hwlib::target::pin_out( hwlib::target::pins::d5 );
    auto MOSI2 = hwlib::target::pin_out( hwlib::target::pins::d4 );
    auto MISO2 = hwlib::target::pin_in( hwlib::target::pins::d6 );
    CSN2.set(1);
    
    auto SPI1 = hwlib::spi_bus_bit_banged_sclk_mosi_miso( SCLK1, MOSI1, MISO1 );
    
    auto SPI2 = hwlib::spi_bus_bit_banged_sclk_mosi_miso( SCLK2, MOSI2, MISO2 );
    
    hwlib::wait_ms(500);
    
    RF24 test1(CE1, CSN1, SPI1);
    
    RF24 test2(CE2, CSN2, SPI2);
    
    hwlib::cout << test1.check_bit(RF_CH, 1);
    
    uint8_t *x;
    
    int y;
    
    //test 1
    
    hwlib::cout << "Test1" << '\n';
    
    y = test1.read_rx_payload_width();
    hwlib::cout << y << '\n';
    
    hwlib::wait_ms(1000);
    
    x = test1.receive();
    hwlib::cout << unsigned(x[0]) << '\n';
    
    x = test1.read_register(RF_CH);
    hwlib::cout << hwlib::bin << unsigned(x[0]) << '\n' << hwlib::bin << unsigned(x[1]) << '\n';
    
    //test1.write_register(RF_CH, 10);
    //test1.set_channel(2);
    test1.set_bit(RF_CH, 3);
    
    hwlib::wait_ms(5000);
    
    x = test1.read_register(RF_CH);
    hwlib::cout << hwlib::bin << unsigned(x[0]) << '\n' << hwlib::bin << unsigned(x[1]) << '\n';
    
//    
//    //test 2
//    
//    hwlib::cout << "Test2" << '\n';
//    
//    y = test2.read_rx_payload_width();
//    hwlib::cout << y << '\n';
//    
//    hwlib::wait_ms(1000);
//    
//    x = test2.receive();
//    hwlib::cout << unsigned(x[0]) << '\n';
//    
//    x = test2.read_register(RF_CH);
//    hwlib::cout << hwlib::bin << unsigned(x[0]) << '\n' << hwlib::bin << unsigned(x[1]) << '\n';
//    
//    test2.set_channel(2);
//    
//    hwlib::wait_ms(5000);
//    
//    x = test2.read_register(RF_CH);
//    hwlib::cout << hwlib::bin << unsigned(x[0]) << '\n' << hwlib::bin << unsigned(x[1]) << '\n';
    
    return 0;
}