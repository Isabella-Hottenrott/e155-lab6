/*
File: Lab_6_JHB.c
Author: Josh Brake
Email: jbrake@hmc.edu
Date: 9/14/19
*/

#include "STM32L432KC.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define LED_PIN PA5
#define BUFF_LEN 32



int main(void) {
configureFlash();
configureClock();

  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);
  
  initTIM(TIM15);

  pinMode(LED_PIN, GPIO_OUTPUT);
  digitalWrite(LED_PIN, 0);
  
//  USART_TypeDef * USART = initUSART(USART1_ID, 125000);
// GPIO enables for SPI
RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
//disable SPI for the configurations
SPI1->CR1 &= ~SPI_CR1_SPE;

SPI1->CR1 |= _VAL2FLD(SPI_CR1_BR, 0b011);   // set the Baud rate (will want 3)
SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, 0);   // set CPOL (will want 0?)// check this acc sets to 0
SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPHA, 1);   // set CPHA (will want 1)// check this acc sets to 0
SPI1->CR1 |= _VAL2FLD(SPI_CR1_BIDIMODE, 0);   // two line unidirectional data mode
SPI1->CR1 |= _VAL2FLD(SPI_CR1_LSBFIRST, 0);   // msb transmitted first

SPI1->CR1 |= _VAL2FLD(SPI_CR1_SSM, 1);   // // check this acc sets to 0
SPI1->CR1 |= _VAL2FLD(SPI_CR1_SSI, 1);   // value forced onto NSS pin
SPI1->CR1 |= _VAL2FLD(SPI_CR1_MSTR, 1);   // master


// to the SPI_CR2 Reg
SPI1->CR2 |= _VAL2FLD(SPI_CR2_DS, 7);
SPI1->CR2 |= _VAL2FLD(SPI_CR2_FRXTH, 1); 
SPI1->CR2 |= _VAL2FLD(SPI_CR2_SSOE, 0);

pinMode(PB3, GPIO_ALT); // PB3 == SCLK want AF5
GPIOB->AFR[0] |= (0b101 << GPIO_AFRL_AFSEL3_Pos);   //AF5
pinMode(PB4, GPIO_ALT); // PB4== MISO want AF5
GPIOB->AFR[0] |= (0b101 << GPIO_AFRL_AFSEL4_Pos);   //AF5
pinMode(PB5, GPIO_ALT); // PB5 = MOSI want AF5
GPIOB->AFR[0] |= (0b101 << GPIO_AFRL_AFSEL5_Pos);   //AF5
pinMode(PB0, GPIO_OUTPUT);                       // PB6 = Chip Select
digitalWrite(PB0, PIO_LOW);


GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED3;


SPI1->CR1 |= SPI_CR1_SPE; 




while(1){
ds1722_init(8);
}

}

