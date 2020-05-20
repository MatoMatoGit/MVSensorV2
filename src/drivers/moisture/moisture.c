/*
 * moisture.c
 *
 * Created: 16-03-2019 22:33:12
 *  Author: dorspi
 */ 

#include "moisture.h"

#include "adc.h"
#include "gpio_moisture.h"

#include <stdlib.h>
#include <util/delay.h>

static ExtInt_t MoistInt = NULL;
static volatile uint16_t PulseCount = 0;



void MoistureInit(ExtInt_t moist_pulse_int)
{
	GpioMoistureInit();

	MoistInt = moist_pulse_int;
}

void MoistureDeinit(void)
{
	GpioMoistureDeinit();
}

uint32_t MoistureSensorReadSingle(void)
{
	GpioMoisturePowerStateSet(1);
	_delay_ms(25);
	ExtIntEnable(MoistInt);
	PulseCount = 0;
	_delay_ms(MOISTURE_CONFIG_MEASURING_TIME_MS);
	
	ExtIntDisable();
	GpioMoisturePowerStateSet(0);
	
	return PulseCount;
}

void MoistureSensorReadBurst(uint8_t n, void (*read_cb)(uint32_t val))
{
	GpioMoisturePowerStateSet(1);
	_delay_ms(25);
	for(uint8_t i = 0; i < n; i++) { 
		PulseCount = 0;
		ExtIntEnable(MoistInt);
		//_delay_ms(MOISTURE_CONFIG_MEASURING_TIME_MS);
		for(volatile uint32_t i = 0; i < 100; i++);
		ExtIntDisable();
		read_cb(PulseCount);
	}
	GpioMoisturePowerStateSet(0);
}

void PulseInterruptHandler(bool state)
{
	PulseCount++;
}