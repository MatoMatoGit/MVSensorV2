#ifndef NOTIFICATION_H_
#define NOTIFICATION_H_

#include <stdint.h>

#define NOTIFICATION_CONFIG_ENABLE_CALIBRATION 0

#define NOTIFICATION_INTERVAL_MS	5e3
#define NOTIFICATION_DURATION_MS	1000
#define CALIBRATION_PERIOD_S		15
#define NOTIFICATION_CALIBRATION_INTERVAL_MS		1000
#define NOTIFICATION_STATE_TRANSITION_INTERVAL_MS	250

typedef enum {
	NOTIFICATION_STATE_COLOR_CYCLE = 0,
	NOTIFICATION_STATE_DETECT_SOIL,
	NOTIFICATION_STATE_CALIBRATE,
	NOTIFICATION_STATE_MEASURE,
	NOTIFICATION_STATE_NOTIFY_ON,
	NOTIFICATION_STATE_NOTIFY_OFF,
	NOTIFICATION_STATE_SLEEP,
}NotificationState_t;

typedef void (*NotificationSleepCallback_t) (uint32_t duration_s);

void NotificationInit(NotificationSleepCallback_t sleep_cb);

NotificationState_t NotificationRun(uint32_t moisture_value);

#endif