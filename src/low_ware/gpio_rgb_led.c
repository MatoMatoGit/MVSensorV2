
#include "gpio_rgb_led.h"

#include <avr/io.h>

#define DDR_RGB_LED		DDRB
#define PORT_RGB_LED	PORTB
#define PIN_LED_RED		PINB1
#define PIN_LED_GREEN	PINB2
#define PIN_LED_BLUE	PINB0

void GpioRgbLedInit(void)
{
	/* Init LED pins as outputs. */
	DDR_RGB_LED |= (1 << PIN_LED_RED) | (1 << PIN_LED_BLUE);
}

void GpioRgbLedRedStateSet(uint8_t state)
{
	if(state) {
		PORT_RGB_LED |= (1 << PIN_LED_RED);
	} else {
		PORT_RGB_LED &= ~(1 << PIN_LED_RED);
	}
}

void GpioRgbLedBlueStateSet(uint8_t state)
{
	if(state) {
		PORT_RGB_LED |= (1 << PIN_LED_BLUE);
	} else {
		PORT_RGB_LED &= ~(1 << PIN_LED_BLUE);
	}
}

void GpioRgbLedGreenStateSet(uint8_t state)
{
	/*
	if(state) {
		PORT_RGB_LED |= (1 << PIN_LED_GREEN);
	} else {
		PORT_RGB_LED &= ~(1 << PIN_LED_GREEN);
	}
	*/
}
