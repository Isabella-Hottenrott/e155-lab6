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

char* webpageStart = "<!DOCTYPE html><html><head><title>E155 Web Server Demo Webpage</title>\
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
	</head>\
	<body><h1>E155 Web Server Demo Webpage</h1>";
char* ledStr = "<p>LED Control:</p><form action=\"ledon\"><input type=\"submit\" value=\"Turn the LED on!\"></form>\
	<form action=\"ledoff\"><input type=\"submit\" value=\"Turn the LED off!\"></form>";
char* webpageEnd   = "</body></html>";

//determines whether a given character sequence is in a char array request, returning 1 if present, -1 if not present
int inString(char request[], char des[]) {
	if (strstr(request, des) != NULL) {return 1;}
	return -1;
}

int updateLEDStatus(char request[])
{
	int led_status = 0;
	// The request has been received. now process to determine whether to turn the LED on or off
	if (inString(request, "ledoff")==1) {
		digitalWrite(LED_PIN, PIO_LOW);
		led_status = 0;
	}
	else if (inString(request, "ledon")==1) {
		digitalWrite(LED_PIN, PIO_HIGH);
		led_status = 1;
	}

	return led_status;
}

int main(void) {
configureFlash();
configureClock();

  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);
  
  initTIM(TIM15);

  pinMode(LED_PIN, GPIO_OUTPUT);
  digitalWrite(LED_PIN, 0);

USART_TypeDef * USART = initUSART(USART1_ID, 125000);

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


  ds1722_init(8);

  while(1) {
    /* Wait for ESP8266 to send a request.
    Requests take the form of '/REQ:<tag>\n', with TAG begin <= 10 characters.
    Therefore the request[] array must be able to contain 18 characters.
    */


    // Receive web request from the ESP
    char request[BUFF_LEN] = "                  "; // initialize to known value
    int charIndex = 0;
  
    // Keep going until you get end of line character
    while(inString(request, "\n") == -1) {
      // Wait for a complete request to be transmitted before processing
      while(!(USART->ISR & USART_ISR_RXNE));
      request[charIndex++] = readChar(USART);
    }
  
    // Update string with current LED state
  int led_status = updateLEDStatus(request);
  int temp = ds1722_read_temp();


    char ledStatusStr[20];
    char tempStr[32];
    
    if (led_status == 1){
      sprintf(ledStatusStr,"LED is on!");
      }
    else if (led_status == 0){
      sprintf(ledStatusStr,"LED is off!");
      sprintf(tempStr, "%.2d", temp);
    }

    // finally, transmit the webpage over UART
    sendString(USART, webpageStart); // webpage header code
    sendString(USART, ledStr); // button for controlling LED

    sendString(USART, "<h2>LED Status</h2>");


    sendString(USART, "<p>");
    sendString(USART, ledStatusStr);
    sendString(USART, "</p>");

    
    sendString(USART, "<h2>Temperature</h2>");
    sendString(USART, "</p>");
    sendString(USART, tempStr);
    sendString(USART, "</p>");
  
    sendString(USART, webpageEnd);


}
}

