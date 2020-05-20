#ifndef NOTIFICATION_TABLE_H_
#define NOTIFICATION_TABLE_H_

#include "notification_mapping.h"
#include <stdint.h>

#define NOTIFICATION_COLOR_TABLE_SIZE 5

#define SEC_TO_MSEC(sec) (sec * 1000)
#define MIN_TO_SEC(min) (min * 60)



const NotificationMapping_t WarningNotificationTable[NOTIFICATION_COLOR_TABLE_SIZE] = {
	{.level = MOISTURE_LEVEL_VERY_DRY, .repeat = 3, .length =  2, .color = RGB_LED_COLOR_RED,  .modes = {RGB_LED_MODE_ON, RGB_LED_MODE_OFF}, .intervals = {200, 200}},
	{.level = MOISTURE_LEVEL_VERY_WET, .repeat = 3, .length =  2, .color = RGB_LED_COLOR_BLUE, .modes = {RGB_LED_MODE_ON, RGB_LED_MODE_OFF}, .intervals = {200, 200}},
};

const NotificationMapping_t StatusNotificationTable[NOTIFICATION_COLOR_TABLE_SIZE] = {
	{.level = MOISTURE_LEVEL_VERY_DRY,	.repeat = 1, .length =  2, .color = RGB_LED_COLOR_RED,  .modes = {RGB_LED_MODE_ON, RGB_LED_MODE_OFF}, .intervals = {500, 500}},
	{.level = MOISTURE_LEVEL_DRY,		.repeat = 1, .length =  2, .color = RGB_LED_COLOR_BLUE, .modes = {RGB_LED_MODE_ON, RGB_LED_MODE_OFF}, .intervals = {500, 500}},
	{.level = MOISTURE_LEVEL_MOIST,		.repeat = 2, .length =  2, .color = RGB_LED_COLOR_BLUE, .modes = {RGB_LED_MODE_ON, RGB_LED_MODE_OFF}, .intervals = {500, 500}},
	{.level = MOISTURE_LEVEL_WET,		.repeat = 3, .length =  2, .color = RGB_LED_COLOR_BLUE, .modes = {RGB_LED_MODE_ON, RGB_LED_MODE_OFF}, .intervals = {500, 500}},
	{.level = MOISTURE_LEVEL_VERY_WET,	.repeat = 4, .length =  2, .color = RGB_LED_COLOR_BLUE, .modes = {RGB_LED_MODE_ON, RGB_LED_MODE_OFF}, .intervals = {500, 500}},
};

#endif