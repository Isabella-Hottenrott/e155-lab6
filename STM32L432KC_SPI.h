// STM32L432KC_SPI.h
// Isabella Hottenrott
// ihottenrott@g.hmc.edu
// 30/10/25

#ifndef STM32L4_SPI_H
#define STM32L4_SPI_H
#include "STM32L432KC_GPIO.h"


#include <stdint.h>
#include <stm32l432xx.h>



///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////


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

/* Enables the SPI peripheral and intializes its clock speed (baud rate), polarity, and phase. */
void initSPI(int br, int cpol, int cpha);

/*Sets up GPIO Port B pins for SPI functionality*/
void SPIGPIO(void);

/* Transmits a character (1 byte) over SPI and returns the received character.
 *    -- send: the character to send over SPI
 *    -- return: the character received over SPI */
uint8_t spiSendReceive(uint8_t send);

#endif