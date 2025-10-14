// STM32L432KC_SPI.h
// TODO: <YOUR NAME>
// TODO: <YOUR EMAIL>
// TODO: <DATE>
// TODO: <SHORT DESCRIPTION OF WHAT THIS FILE DOES>

#ifndef STM32L4_SPI_H
#define STM32L4_SPI_H

#include <stdint.h>
#include <stm32l432xx.h>


///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

// want this to be master
// want full duplex communication
//41.4.5 look at NSS if needed


// Values which "val" can take on in digitalWrite()
#define CLK2  0 // half fpclk
#define CLK4  1 // quarter fpclk
#define CLK8  2 // eighth fpclk
#define CLK16  3 // sixteenth fpclk
#define CLK32  4 // 32th fpclk
#define CLK64  5 // 64th fpclk
#define CLK128  6 // 128th fpclk
#define CLK256  7 // 256th fpclk


#define Inactive0    0
#define Inactive1    1
#define CAP_Lead    0
#define CHNG_Lead    1



///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

/* Enables the SPI peripheral and intializes its clock speed (baud rate), polarity, and phase.
 *    -- br: (0b000 - 0b111). The SPI clk will be the master clock / 2^(BR+1).
 *    -- cpol: clock polarity (0: inactive state is logical 0, 1: inactive state is logical 1).
 *    -- cpha: clock phase (0: data captured on leading edge of clk and changed on next edge, 
 *          1: data changed on leading edge of clk and captured on next edge)
 * Refer to the datasheet for more low-level details. */ 
void initSPI(int br, int cpol, int cpha);

void SPIGPIO(void);

/* Transmits a character (1 byte) over SPI and returns the received character.
 *    -- send: the character to send over SPI
 *    -- return: the character received over SPI */
char spiSendReceive(char send);

#endif