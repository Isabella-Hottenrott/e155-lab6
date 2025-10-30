// Isabella Hottenrott
// ihottenrott@g.hmc.edu
// 30/10/25
// DS1722.h



#ifndef DS1722_H
#define DS1722_H

#include <stdint.h>
#include <stm32l432xx.h>
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_SPI.h"


void ds1722_init(int resolution);
int16_t ds1722_read_temp(void);


#endif