// Isabella Hottenrott
// ihottenrott@g.hmc.edu
// 30/10/25
// DS1722.c

#include "DS1722.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_SPI.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>


//Configure the Temperature sensor by writing to the 0x80 address
void ds1722_init(int config){
    uint8_t ds1722_cfg_addr = 0x80;
    uint8_t readconfig = 0x00;

    digitalWrite(PB0, PIO_HIGH);               //Set CE high to read
    spiSendReceive(ds1722_cfg_addr);           // Send config address
    spiSendReceive(config);                    // Send config selection   
    digitalWrite(PB0, PIO_LOW);

   // digitalWrite(PB0, PIO_HIGH);              //Set CE high to read
   // uint8_t config2 = spiSendReceive(0x00);   //Optional Readback address
   // uint8_t config3 = spiSendReceive(0x00);   //Optional Optional return Readback
   // digitalWrite(PB0, PIO_LOW); 
}


//Read the DS1722 Temperature by asserting and deasserting CE twice: once for MSB reading, onece for LSB reading
//Return the combined read value as a single 16-bit output
int16_t ds1722_read_temp(void){
    digitalWrite(PB0, PIO_HIGH);               // Set CE high
    spiSendReceive(0x02);
    uint8_t upperhalf = spiSendReceive(0x02);
    digitalWrite(PB0, PIO_LOW);               //Set CE low

    digitalWrite(PB0, PIO_HIGH);              //Set CE high
    spiSendReceive(0x01);
    uint8_t lowerhalf = spiSendReceive(0x01);
    digitalWrite(PB0, PIO_LOW);               //Set CE low

   
    int16_t raw = ((int16_t)upperhalf << 8) | lowerhalf; 
    return raw;

}