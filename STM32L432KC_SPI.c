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
SPI1->CR1 &= ~(0b1);
SPI1->CR2 &= ~(0b1); 

SPI1->CR1 |= _VAL2FLD(SPI_CR1_BR, br);   // set the Baud rate (will want 3)
SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol);   // set CPOL (will want 0?)
SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPHA, cpha);   // set CPHA (will want 0?)
SPI1->CR1 |= _VAL2FLD(SPI_CR1_BIDIMODE, 0);   // two line unidirectional data mode
SPI1->CR1 |= _VAL2FLD(SPI_CR1_LSBFIRST, 0);   // msb transmitted first
        // configuretheCRCL and CRCEN bits if CRC isneeded
SPI1->CR1 |= _VAL2FLD(SPI_CR1_SSM, 1);   // slave select info driven by SSI
//! !!!!!!!!! DO I WANT TO HAVE 1 DOWN HERE??
SPI1->CR1 |= _VAL2FLD(SPI_CR1_SSI, 1);   // value forced onto NSS pin
SPI1->CR1 |= _VAL2FLD(SPI_CR1_MSTR, 1);   // msb transmitted first


// to the SPI_CR2 Reg
SPI1->CR2 |= _VAL2FLD(SPI_CR2_DS, 7);
SPI1->CR2 |= _VAL2FLD(SPI_CR2_FRXTH, 1); 
SPI1->CR2 |= _VAL2FLD(SPI_CR2_SSOE, 0); 

SPI1->CR1 |= (0b1); 
}

void SPIGPIO(){
// GPIO enables for SPI
RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
pinMode(PA1, GPIO_ALT); // PA1 == SCLK want AF5
setAF(PA1);
pinMode(PA10, GPIO_ALT); // PA10 = NSS  want AF5
setAF(PA10);
pinMode(PA6, GPIO_ALT); // PA6== MISO want AF5
setAF(PA6);
pinMode(PA7, GPIO_ALT); // PA7 = MOSI want AF5
setAF(PA7);

pinMode(PA4, GPIO_OUTPUT); // PA4 = Chip Select
GPIOA->OTYPER &= ~(0b1<<4); // Push pull
GPIOA->ODR |= (0b1<<4); // set initial logic level to high (active low)

}


// Reading the SPI1_DR register returns the oldest val stored in RXFIFOthat has 
// no yet been read. A write access to SPI1_DR storesthe written data in the TXFIFOat the end of a send
// The readaccess must always be aligned with the RXFIFO threshold configuredby the FRXTH bit in SPI1_CR2

//A read access to SPI1_DR must be managed by the RXNE event
// Event is triggeredwhen data isstored inRXFIFO and threshold (FRXTH) is reached
//When RXNE is cleared, RXFIFO isconsideredempty

// write access of a data frame to be transmitted is managed by the TXE event.
// TXE is triggered when the TXFIFO level is less than or equal to half of capacity
// Otherwise TXE is cleared and TXFIFO isconsidered full
// THereforeRXFIFO can store up to four data frames, but TX FIFO can only store up tothree
// makes corrption impossible
// TXE and RXNEcanbe polled or handled by interrupts

// it is preferable fo rhte slave to use DMA, expecially when data frames are shorter and the bus rate is high

// Read disapling SPI procedure from page 1337

char spiSendReceive(char send){
    while (!(SPI1->SR & SPI_SR_TXE)); // wait until TX FIFO is empty

    *((volatile uint8_t *)&SPI1->DR) = (uint8_t)send; // Send a byte

    while (!(SPI1->SR & SPI_SR_RXNE)); // wait until received

    return *((volatile uint8_t *)&SPI1->DR); // returnthe received


}