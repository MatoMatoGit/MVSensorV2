#ifndef NOTIFICATION_H_
#define NOTIFICATION_H_

#include "notification_mapping.h"
#include <stdint.h>

#define NOTIFICATION_CONFIG_ENABLE_CALIBRATION 0

#define NOTIFICATION_INTERVAL_MS	5e3
#define NOTIFICATION_DURATION_MS	1000
#define CALIBRATION_PERIOD_S		15
#define NOTIFICATION_CALIBRATION_INTERVAL_MS		1000
#define NOTIFICATION_STATE_TRANSITION_INTERVAL_MS	250

/*
typedef enum {
	NOTIFICATION_STATE_COLOR_CYCLE = 0,
	NOTIFICATION_STATE_DETECT_SOIL,
	NOTIFICATION_STATE_CALIBRATE,
	NOTIFICATION_STATE_MEASURE,
	NOTIFICATION_STATE_NOTIFY_ON,
	NOTIFICATION_STATE_NOTIFY_OFF,
	NOTIFICATION_STATE_SLEEP,
}NotificationState_e;
*/

typedef enum {
	NOTIFICATION_STATE_AWAIT_INPUT = 0,
	NOTIFICATION_STATE_BUSY,
	NOTIFICATION_STATE_DONE,
}NotificationState_e;

typedef void (*NotificationSleepCallback_t) (uint32_t duration_s);

void NotificationInit(NotificationSleepCallback_t sleep_cb);

NotificationState_e NotificationRun(int16_t moisture_value, NotificationMapping_t *mapping);

#endif