/*
 * gpio_moisture.h
 *
 * Created: 16-03-2019 22:41:33
 *  Author: dorspi
 */ 


#ifndef GPIO_MOISTURE_H_
#define GPIO_MOISTURE_H_

#include <stdint.h>

void GpioMoistureInit(void);

void GpioMoisturePowerStateSet(uint8_t state);

#endif /* GPIO_MOISTURE_H_ */