/*
 * sensor.c
 *
 * Created: 16-03-2019 22:47:46
 *  Author: dorspi
 */ 

#include "sensor.h"

#include "oc_timer.h"
#include "moisture.h"
#include "filter.h"

#include "SoftwareSerial.h"

#if SENSOR_ENABLE_PERIODIC_READ==1
static void OcTimerCallback(void);
#else
static void SensorReadCallback(uint32_t value);
#endif

static FilterDataType_t moisture_filter_buf[SENSOR_FILTER_DEPTH];

static Filter_t moisture_filter = {
	.n = SENSOR_FILTER_DEPTH,
	.buffer = moisture_filter_buf
};

void SensorInit(void)
{
	FilterInit(&moisture_filter);

#if SENSOR_ENABLE_PERIODIC_READ==1
	OcTimerInit(244);
	OcTimerCallbackRegister(OcTimerCallback);
	OcTimerStart();
#endif
}

uint32_t SensorValueGet(void)
{
	uint32_t value = 0;

#if SENSOR_ENABLE_PERIODIC_READ==0
	MoistureSensorReadBurst(SENSOR_FILTER_DEPTH, SensorReadCallback);
#endif

	FilterOut(&moisture_filter, &value);
	
	return value;
}

#if SENSOR_ENABLE_PERIODIC_READ==1
static void OcTimerCallback(void)
{
	uint32_t value = MoistureSensorReadSingle();
	FilterIn(&moisture_filter, value);
	//softSerialPrintInt(value);
	//softSerialPrintLn("");
}
#else
static void SensorReadCallback(uint32_t value)
{
	FilterIn(&moisture_filter, value);
}
#endif