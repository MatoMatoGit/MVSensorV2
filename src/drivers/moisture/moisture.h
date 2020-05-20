/*
 * moisture.h
 *
 * Created: 16-03-2019 22:32:22
 *  Author: dorspi
 */ 


#ifndef MOISTURE_H_
#define MOISTURE_H_

#include "ext_int.h"

#include <stdint.h>

#define MOISTURE_CONFIG_MEASURING_TIME_MS 2000//50
#define MOISTURE_CONFIG_USE_

void MoistureInit(ExtInt_t moist_pulse_int);

void MoistureDeinit(void);

uint32_t MoistureSensorReadSingle(void);

void MoistureSensorReadBurst(uint8_t n, void (*read_cb)(uint32_t val));

void PulseInterruptHandler(bool state);

#endif /* MOISTURE_H_ */