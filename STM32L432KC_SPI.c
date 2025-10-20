// STM32L432KC_SPI.c


#include "STM32L432KC_RCC.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_SPI.h"

/* Enables the SPI peripheral and intializes its clock speed (baud rate), polarity, and phase.
 *    -- br: (0b000 - 0b111). The SPI clk will be the master clock / 2^(BR+1).
 *    -- cpol: clock polarity (0: inactive state is logical 0, 1: inactive state is logical 1).
 *    -- cpha: clock phase (0: data captured on leading edge of clk and changed on next edge, 
 *          1: data changed on leading edge of clk and captured on next edge)
 * Refer to the datasheet for more low-level details. */ 


void initSPI(int br, int cpol, int cpha){

SPIGPIO();

RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
//disable SPI for the configurations
SPI1->CR1 &= ~SPI_CR1_SPE;

SPI1->CR1 |= _VAL2FLD(SPI_CR1_BR, br);   // set the Baud rate (will want 3)
SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol);   // set CPOL (will want 0?)// check this acc sets to 0
SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPHA, cpha);   // set CPHA (will want 1)// check this acc sets to 0
SPI1->CR1 |= _VAL2FLD(SPI_CR1_BIDIMODE, 0);   // two line unidirectional data mode
SPI1->CR1 |= _VAL2FLD(SPI_CR1_LSBFIRST, 0);   // msb transmitted first

SPI1->CR1 |= _VAL2FLD(SPI_CR1_SSM, 1);   // // check this acc sets to 0
SPI1->CR1 |= _VAL2FLD(SPI_CR1_SSI, 1);   // value forced onto NSS pin
SPI1->CR1 |= _VAL2FLD(SPI_CR1_MSTR, 1);   // master


// to the SPI_CR2 Reg
SPI1->CR2 |= _VAL2FLD(SPI_CR2_DS, 7);
SPI1->CR2 |= _VAL2FLD(SPI_CR2_FRXTH, 1); 
SPI1->CR2 |= _VAL2FLD(SPI_CR2_SSOE, 0);
SPI1->CR1 |= SPI_CR1_SPE; 
}

void SPIGPIO(){
// GPIO enables for SPI
RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

pinMode(PB3, GPIO_ALT); // PB3 == SCLK want AF5
GPIOB->AFR[0] |= (0b101 << GPIO_AFRL_AFSEL3_Pos);   //AF5
pinMode(PB4, GPIO_ALT); // PB4== MISO want AF5
GPIOB->AFR[0] |= (0b101 << GPIO_AFRL_AFSEL4_Pos);   //AF5
pinMode(PB5, GPIO_ALT); // PB5 = MOSI want AF5
GPIOB->AFR[0] |= (0b101 << GPIO_AFRL_AFSEL5_Pos);   //AF5
pinMode(PB0, GPIO_OUTPUT);                       // PB6 = Chip Select
digitalWrite(PB0, PIO_LOW);

//GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED3;
}




// Read disapling SPI procedure from page 1337

uint8_t spiSendReceive(uint8_t send){
    while (!(SPI1->SR & SPI_SR_TXE)); // wait until TX FIFO is empty
    *(volatile uint8_t *) (&SPI1->DR) = send; // Send a byte
    while (!(SPI1->SR & SPI_SR_RXNE));
    uint8_t r = *(volatile uint8_t *)&SPI1->DR;
    return r;
}

