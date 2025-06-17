#ifndef my_uhr_h
#define my_uhr_h 1

#include "ili_9341/ili_9341.h"
#include "buttons/buttons.h"
#include "pico/stdlib.h"
#include "pico/double.h"
#include "pico/malloc.h"
#include "pico/util/datetime.h"
#include "hardware/i2c.h"
#include "hardware/rtc.h"
#include <stdlib.h>

#define STD_GRAD 30
#define MIN_GRAD 6

uint8_t Adr = 0x68;
uint16_t StdColor = 0xFFFF;
uint16_t MinColor = 0xFFFF;
uint16_t ClrColor = 0x0000;
uint8_t Abstand = 6; // Abstand Zeischen Minutenzeiger und Ziffernblattstrichen
uint8_t ZifferWidth = 1;
uint8_t ZifferLen = 15;
uint8_t StdZeiger_width = 5;
uint8_t MinZeiger_width = 1;;
uint8_t ZeigerDiff = 30;

void uhr_init(uint16_t x, uint16_t y, uint16_t width, uint8_t bus, uint8_t sda, uint8_t scl);

#endif
