/* Defines pinouts for the LoRa and SPI boards connected
* to the ESP32 board of the groundstation
*/

#ifndef EVE_PINS_H
#define EVE_PINS_H

#ifdef PROTOTYPE

    #define FSPI_SCLK_PIN   37
    #define FSPI_MISO_PIN   36
    #define FSPI_MOSI_PIN   35
    
    #define RFM_CS_PIN 38
    #define RFM_RST_PIN 7
    #define RFM_IRQ_PIN 33

    #define SD_CS_PIN 10

#endif

#endif