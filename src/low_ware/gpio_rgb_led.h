#ifndef GPIO_RGB_LED_H_
#define GPIO_RGB_LED_H_

#include <stdint.h>

void GpioRgbLedInit(void);

void GpioRgbLedRedStateSet(uint8_t state);
void GpioRgbLedBlueStateSet(uint8_t state);
void GpioRgbLedGreenStateSet(uint8_t state);

#endif
