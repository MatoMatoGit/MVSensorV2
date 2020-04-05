/*
 * moisture.h
 *
 * Created: 16-03-2019 22:32:22
 *  Author: dorspi
 */ 


#ifndef MOISTURE_H_
#define MOISTURE_H_

#include <stdint.h>

#define MOISTURE_CONFIG_ENABLE_LOW_POWER_MODE

void MoistureInit(void);

void MoistureDeinit(void);

uint32_t MoistureSensorReadSingle(void);

void MoistureSensorReadBurst(uint8_t n, void (*read_cb)(uint32_t val));

#endif /* MOISTURE_H_ */