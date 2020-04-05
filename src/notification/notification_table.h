#ifndef NOTIFICATION_TABLE_H_
#define NOTIFICATION_TABLE_H_

#include "rgb_led.h"
#include "sensor.h"

#include <stdint.h>

#define NOTIFICATION_COLOR_TABLE_SIZE 3
#define NOTIFICATION_TABLE_TEST

#define SEC_TO_MSEC(sec) (sec * 1000)
#define MIN_TO_SEC(min) (min * 60)

typedef struct {
	uint32_t value;
	RgbLedColor_t color;
	uint32_t time_sleep;
	uint32_t time_notify;
}NotificationMapping_t;

typedef enum {
	MOISTURE_LEVEL_VERY_DRY = 0,
	MOISTURE_LEVEL_DRY,
	MOISTURE_LEVEL_PERFECT,
	MOISTURE_LEVEL_WET,
	MOISTURE_LEVEL_VERY_WET,
} MoistureLevel_t;

#ifndef  NOTIFICATION_TABLE_TEST
/* Production mapping. */
NotificationMapping_t NotificationTable[NOTIFICATION_COLOR_TABLE_SIZE] = {
	{.value = SENSOR_MIN_VALUE, .color = RGB_LED_COLOR_RED,	.time_sleep = 60, .time_notify = SEC_TO_MSEC(1)},,
	{.value = 250, .color = RGB_LED_COLOR_GREEN, .time_sleep = 60, .time_notify = SEC_TO_MSEC(1)},
	{.value = 745, .color = RGB_LED_COLOR_BLUE, .time_sleep = 60, .time_notify = SEC_TO_MSEC(1)},
};
#else
/* Test mapping. */
NotificationMapping_t NotificationTable[NOTIFICATION_COLOR_TABLE_SIZE] = {
	{.value = SENSOR_MIN_VALUE, .color = RGB_LED_COLOR_RED,	.time_sleep = 2, .time_notify = SEC_TO_MSEC(1)},
	{.value = 250, .color = RGB_LED_COLOR_GREEN, .time_sleep = 2, .time_notify = SEC_TO_MSEC(1)},
	{.value = 745, .color = RGB_LED_COLOR_BLUE, .time_sleep = 2, .time_notify = SEC_TO_MSEC(1)},
};
#endif

#endif