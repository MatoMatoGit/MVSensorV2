#ifndef NOTIFICATION_H_
#define NOTIFICATION_H_

#include "notification_mapping.h"
#include <stdint.h>

#define NOTIFICATION_STATE_TRANSITION_INTERVAL_MS	250

typedef enum {
	NOTIFICATION_STATE_AWAIT_INPUT = 0,
	NOTIFICATION_STATE_BUSY,
	NOTIFICATION_STATE_DONE,
}NotificationState_e;

void NotificationInit(void);

NotificationState_e NotificationRun(MoistureLevel_e moisture_level, NotificationMapping_t *mapping, uint8_t table_size);

#endif