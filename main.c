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
#include <math.h>

#define LED_PIN PA5
#define BUFF_LEN 64
int config;


char* webpageStart = "<!DOCTYPE html><html><head><title>Bella's Temperature Sensor</title>\
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
	</head>\
	<body><h1>Bella's Temperature Sensor</h1>";

char* ledStr = "<p>LED Control:</p><form action=\"ledon\"><input type=\"submit\" value=\"Turn the LED on!\"></form>\
	<form action=\"ledoff\"><input type=\"submit\" value=\"Turn the LED off!\"></form>";

char* Resolution = "<p>Resolution:</p><form action=\"8-bit\"><input type=\"submit\" value=\"8-bit\"></form>\
	<form action=\"9-bit\"><input type=\"submit\" value=\"9-bit\"></form>\
	<form action=\"10-bit\"><input type=\"submit\" value=\"10-bit\"></form>\
	<form action=\"11-bit\"><input type=\"submit\" value=\"11-bit\"></form>\
	<form action=\"12-bit\"><input type=\"submit\" value=\"12-bit\"></form>";

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

void resolution(char request[])
{
        
	if (inString(request, "8-bit")==1) {
                config = 0xE0;
	}
	else if (inString(request, "9-bit")==1) {
                config = 0xE2;
	}
        else if (inString(request, "10-bit")==1) {
                config = 0xE4;
	}
        else if (inString(request, "11-bit")==1) {
                config = 0xE6;
	}else if (inString(request, "12-bit")==1) {
                config = 0xEE;
        }
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

initSPI(3,0,1);


while(1){
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
  resolution(request);
  int led_status = updateLEDStatus(request);

  ds1722_init(config);

  int16_t temp = ds1722_read_temp();


    char ledStatusStr[20];
    char tempStr[32];
    char resStatusStr[24];

    if (led_status == 1)
      sprintf(ledStatusStr,"LED is on!");
    else if (led_status == 0)
      sprintf(ledStatusStr,"LED is off!");

    if (config == 0xE0)
      sprintf(resStatusStr, "8-bit");
    else if (config == 0xE2)
      sprintf(resStatusStr, "9-bit");
    else if (config == 0xE4)
      sprintf(resStatusStr, "10-bit");
    else if (config == 0xE6)
      sprintf(resStatusStr, "11-bit");
    else if (config == 0xEE)
      sprintf(resStatusStr, "12-bit");


    
    int16_t wholenum = temp >> 8;          // high byte → integer part (sign-extended)
    uint8_t fracnum = (temp >> 4) & 0x0F; // low nibble → fraction bits
    float frac = fracnum * 0.0625f;      // each bit = 1/16 °C
    float tempC = (float)wholenum + frac;

    sprintf(tempStr, "%.4f", tempC);

//or whole= %d frac float= %f, frac uint= %d
//wholenum, frac, fracnum

    // finally, transmit the webpage over UART
    sendString(USART, webpageStart); // webpage header code
    sendString(USART, ledStr); // button for controlling LED
    sendString(USART, Resolution); // button for controlling Resolution

    sendString(USART, "<h2>LED Status</h2>");
    sendString(USART, "<p>");
    sendString(USART, ledStatusStr);
    sendString(USART, "</p>");

    sendString(USART, "<h2>Resolution Status</h2>");  
    sendString(USART, "<p>");                        
    sendString(USART, resStatusStr);                 
    sendString(USART, "</p>");      

    
    sendString(USART, "<h2>Temperature</h2>");
    sendString(USART, "</p>");
    sendString(USART, tempStr);
    sendString(USART, "</p>");
  
    sendString(USART, webpageEnd);
}

}

