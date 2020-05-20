/*
 * gpio_moisture.c
 *
 * Created: 16-03-2019 22:42:28
 *  Author: dorspi
 */

#include "gpio_moisture.h"

void GpioMoistureInit(void)
{
	MOISTURE_DDR |= (1 << MOISTURE_EN_PIN);
	MOISTURE_DDR &= ~(1 << MOISTURE_OUT_PIN);
	MOISTURE_PORT |= (1 << MOISTURE_OUT_PIN);
}

void GpioMoistureDeinit(void)
{
	MOISTURE_DDR |= (1 << MOISTURE_OUT_PIN);
	MOISTURE_PORT &= ~(1 << MOISTURE_OUT_PIN);
	MOISTURE_PORT &= ~(1 << MOISTURE_EN_PIN);
}


void GpioMoisturePowerStateSet(uint8_t state)
{
	if(state) {
		MOISTURE_PORT |= (1 << MOISTURE_EN_PIN);
	} else {
		MOISTURE_PORT &= ~(1 << MOISTURE_EN_PIN);
	}
}