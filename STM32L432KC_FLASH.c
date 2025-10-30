// STM32L432KC_FLASH.c
// Isabella Hottenrott
// ihottenrott@g.hmc.edu
// 30/10/25
// Source code for FLASH functions

#include "STM32L432KC_FLASH.h"

void configureFlash() {
  FLASH->ACR |= FLASH_ACR_LATENCY_4WS;
  FLASH->ACR |= FLASH_ACR_PRFTEN;
}