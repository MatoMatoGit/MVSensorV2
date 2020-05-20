/*
 * gpio_moisture.h
 *
 * Created: 16-03-2019 22:41:33
 *  Author: dorspi
 */ 


#ifndef GPIO_MOISTURE_H_
#define GPIO_MOISTURE_H_

#include <stdint.h>
#include <avr/io.h>

#define MOISTURE_DDR	DDRB
#define MOISTURE_PORT	PORTB
#define MOISTURE_EN_PIN	PINB3
#define MOISTURE_OUT_PIN	PINB4

void GpioMoistureInit(void);
void GpioMoistureDeinit(void);

void GpioMoisturePowerStateSet(uint8_t state);

#endif /* GPIO_MOISTURE_H_ */