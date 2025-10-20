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

#define LED_PIN PA5
#define BUFF_LEN 32

char* webpageStart = "<!DOCTYPE html><html><head><title>E155 Web Server Demo Webpage</title>\
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
	</head>\
	<body><h1>E155 Web Server Demo Webpage</h1>";

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

int resolution(char request[])
{
	int config = 0xE0;
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

	return config;
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
ds1722_init(0xE0);


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

  int led_status = updateLEDStatus(request);
  int config = resolution(request);
  ds1722_init(config);

  int temp = ds1722_read_temp();
  //delay_millis(TIM15, 100);




    char ledStatusStr[20];
    char tempStr[32];
    


    // finally, transmit the webpage over UART
    sendString(USART, webpageStart); // webpage header code
    sendString(USART, ledStr); // button for controlling LED
    sendString(USART, Resolution); // button for controlling Resolution

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

