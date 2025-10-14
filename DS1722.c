// DS1722.c

#include "DS1722.h"
#include "DS1722.h"
#include "STM32L432KC_GPIO.h"

// want to config SPI slave before themaster sends a clock
// to avoid undesired data transmission

// Master at full-duplex starts to communicate whent heSPI 
// is enabled and TXFIFO is not empty, or with the next write to TXFIFO

// DS1722 continuously CONVERTS in the background (after configuring it)
// but neveractually drivesthe spi bus by itself

void ds1722_init(int resolution){
    uint8_t cfg = 0xE0;
    uint8_t DS1722_WR_CFG = 0x80;

    
    digitalWrite(22, PIO_HIGH); //pin PB6 is chip enable pin. set high
    while(!(SPI1->SR & SPI_SR_TXE)) { // same as writing while (!(SPI1->SR & 0b10)) { }
    }
    *(volatile uint8_t*)&SPI1->DR = DS1722_WR_CFG;
    while(!(SPI1->SR & SPI_SR_RXNE)) {
    }
    while(!(SPI1->SR & SPI_SR_TXE)) {
    }
    *(volatile uint8_t*)&SPI1->DR = cfg;
    while(!(SPI1->SR & SPI_SR_RXNE)) {
    }
    digitalWrite(22, PIO_HIGH); //pin PB6 is chip enable pin. set high

    // could try to read here to make sure correct config
}

int ds1722_read_temp(void){

    digitalWrite(22, PIO_HIGH); //pin PB6 is chip enable pin. set high
    //send read address for LSB:
    while(!(SPI1->SR & SPI_SR_TXE)) {}
    *(volatile uint8_t*)&SPI1->DR = 0x01; // send read address for LSB reg
    while(!(SPI1->SR & SPI_SR_RXNE)) {}
    // clockout one byte
    while(!(SPI1->SR & SPI_SR_TXE)) {}
    *(volatile uint8_t*)&SPI1->DR = 0xFF; // send trash read address
    while(!(SPI1->SR & SPI_SR_RXNE)) {}
    uint8_t lowerhalf = *(volatile uint8_t*)&SPI1->DR;   //put the LSB into lowerhalf var
    digitalWrite(22, PIO_LOW); //pin PB6 is chip enable pin. set high


    digitalWrite(22, PIO_HIGH); //pin PB6 is chip enable pin. set high
    //send read address for LSB:
    while(!(SPI1->SR & SPI_SR_TXE)) {}
    *(volatile uint8_t*)&SPI1->DR = 0x02; // send read address for MSB reg
    while(!(SPI1->SR & SPI_SR_RXNE)) {}
    // clockout one byte
    while(!(SPI1->SR & SPI_SR_TXE)) {}
    *(volatile uint8_t*)&SPI1->DR = 0xFF; // send trash read address
    while(!(SPI1->SR & SPI_SR_RXNE)) {}
    uint8_t upperhalf = *(volatile uint8_t*)&SPI1->DR;   //put the LSB into lowerhalf var
    digitalWrite(22, PIO_LOW); //pin PB6 is chip enable pin. set high

   
    int16_t raw = ((int16_t)upperhalf << 8) | lowerhalf; // keep sign by shifting arithmetic
    raw = raw >> 8;                       // shift by 8 because we only have 8 bit resolution but a 16 bit representation
    raw = raw *0.0625;
    return raw;
}


//for serial interface: must connect Vddd to SERMODE pin
// to select SPI communication mode
// cpha on mcu must be set to 1
// spi shifts data bytes msb first
// the msb of any SDI will determine if a read
// or write is taking place (0=read, 1=write)