/*
 * sensor.h
 *
 * Created: 16-03-2019 22:38:51
 *  Author: dorspi
 */ 


#ifndef SENSOR_H_
#define SENSOR_H_

#include <stdint.h>

#define SENSOR_FILTER_DEPTH		5
#define SENSOR_READ_INTERVAL_MS 1000
#define SENSOR_ENABLE_PERIODIC_READ 0

#define SENSOR_MIN_VALUE 10
#define SENSOR_MAX_VALUE 900

void SensorInit(void);

uint32_t SensorValueGet(void);

#endif /* SENSOR_H_ */