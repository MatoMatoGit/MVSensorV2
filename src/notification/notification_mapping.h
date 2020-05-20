/*
 * notification_mapping.h
 *
 * Created: 19-05-2020 22:41:48
 *  Author: dorspi
 */ 


#ifndef NOTIFICATION_MAPPING_H_
#define NOTIFICATION_MAPPING_H_

#include "rgb_led.h"
#include "moisture_level.h"

#define NOTIFICATION_SEQUENCE_MAX_LEN 3

typedef struct {
	MoistureLevel_e level;
	uint8_t repeat;
	uint8_t length;
	RgbLedColor_t color;
	RgbLedMode_t  modes[NOTIFICATION_SEQUENCE_MAX_LEN];
	uint32_t intervals[NOTIFICATION_SEQUENCE_MAX_LEN];
}NotificationMapping_t;



#endif /* NOTIFICATION_MAPPING_H_ */