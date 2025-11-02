/*
// Isabella Hottenrott
// ihottenrott@g.hmc.edu
// 30/10/25
// main.c
*/

#include "STM32L432KC.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define LED_PIN PA5
#define BUFF_LEN 64



int main(void) {
configureFlash();
configureClock();

  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);
  

  pinMode(LED_PIN, GPIO_OUTPUT);
  digitalWrite(LED_PIN, 0);
  

initSPI(3,0,1);


 int config = 0xEE;

 ds1722_init(config);

 int16_t temp = ds1722_read_temp();
    int16_t wholenum = temp >> 8;         // MSB is in top 8 bits of temp
    uint8_t fracnum = (temp >> 4)&0x0F;   // relevant LSB is in 4-to-last bits of temp. Mask just in case
    float frac = fracnum * 0.0625f;      // each fractional bit = 1/16 °C
    float tempC = (float)wholenum + frac;

    printf("%.4f degrees Celsius", tempC);

    while(1);


}

