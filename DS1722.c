// DS1722.c

#include "DS1722.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_SPI.h"
#include <stdio.h>


// want to config SPI slave before themaster sends a clock
// to avoid undesired data transmission

// Master at full-duplex starts to communicate whent heSPI 
// is enabled and TXFIFO is not empty, or with the next write to TXFIFO

// DS1722 continuously CONVERTS in the background (after configuring it)
// but neveractually drivesthe spi bus by itself

void ds1722_init(int resolution){
    uint8_t cfg = 0xE0;
    uint8_t ds1722_cfg_addr = 0x80;
    uint8_t readconfig = 0x00;
    digitalWrite(PB6, PIO_HIGH); //pin PB6 is chip enable pin. set high
    uint8_t trash1 = spiSendReceive(ds1722_cfg_addr);
    uint8_t trash2 = spiSendReceive(cfg);
    while (SPI1->SR & SPI_SR_BSY);
    digitalWrite(PB6, PIO_LOW); //pin PB6 is chip enable pin. set low

    digitalWrite(PB6, PIO_HIGH); //pin PB6 is chip enable pin. set high
    uint8_t config1 = spiSendReceive(readconfig);
    uint8_t config2 = spiSendReceive(readconfig);
    while (SPI1->SR & SPI_SR_BSY);    // be safe before CE↓
    digitalWrite(PB6, PIO_LOW); //pin PB6 is chip enable pin. set low
    printf("%d", config2);
}


int ds1722_read_temp(void){
    digitalWrite(PB6, PIO_HIGH); //pin PB6 is chip enable pin. set high
    uint8_t trash1 = spiSendReceive(0x01);
    uint8_t lowerhalf = spiSendReceive(0x01);

    uint8_t trash2 = spiSendReceive(0x02);
    uint8_t upperhalf = spiSendReceive(0x02);
    digitalWrite(PB6, PIO_LOW); //pin PB6 is chip enable pin. set low
    
   // printf("%d", lowerhalf);
   // printf("%d", upperhalf);

   
    int16_t raw = ((int16_t)upperhalf << 8) | lowerhalf; // keep sign by shifting arithmetic
    raw = raw >> 8;                       // shift by 8 because we only have 8 bit resolution but a 16 bit representation
    raw = raw *0.0625;
    return raw;
}
