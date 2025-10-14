// DS1722.h



#ifndef DS1722_H
#define DS1722_H

#include <stdint.h>
#include <stm32l432xx.h>

void ds1722_init(int resolution);
int ds1722_read_temp(void);

#endif