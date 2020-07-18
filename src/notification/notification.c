#include "notification.h"
#include "notification_mapping.h"

/* Drivers. */
#include "rgb_led.h"
#include "soft_timer.h"
#include "sleep.h"

/* Serial. */
#include "SoftwareSerial.h"

/* System libs. */
#include <util/delay.h>
#include <avr/pgmspace.h>

/* Standard libs. */
#include <stdint.h>

static void NotificationTimerRun(uint32_t t_ms);
static void NotificationTimerCallback(SoftTimer_t timer);
static NotificationMapping_t *INotificationMappingFromMoistureLevel(MoistureLevel_e level);
static const char *INotificationColorToString(RgbLedColor_t color);

static volatile NotificationState_e CurrentState;
static SoftTimer_t NotificationTimer;
static volatile uint8_t TimerFlag = 0;
static NotificationMapping_t *NotificationMapping = NULL;
static uint8_t NotificationTableSize = 0;

void NotificationInit(void)
{
	CurrentState = NOTIFICATION_STATE_AWAIT_INPUT;

	RgbLedModeSet(RGB_LED_MODE_OFF);
	NotificationTimer = SoftTimerCreate(NOTIFICATION_STATE_TRANSITION_INTERVAL_MS, NotificationTimerCallback);
	TimerFlag = 1;
}

NotificationState_e NotificationRun(MoistureLevel_e moisture_level, NotificationMapping_t *mapping, uint8_t table_size)
{
	if(TimerFlag == 0 || mapping == NULL) {
		return CurrentState;
	}
	
	static uint8_t notification_idx = 0;
	static NotificationMapping_t *active_notification = NULL;
	
	NotificationMapping = mapping;
	NotificationTableSize = table_size;
	
	softSerialPrintInt(CurrentState);
	softSerialPrintLn("");
	
	switch(CurrentState) {
	
	case NOTIFICATION_STATE_AWAIT_INPUT:
		//softSerialPrintInt(moisture_value);
		//softSerialPrint(", ");
		
		/* Map the sensor value to a color sequence. */
		active_notification = INotificationMappingFromMoistureLevel(moisture_level);
		if(active_notification == NULL) {
			softSerialPrintLn("No mapping found");
			CurrentState = NOTIFICATION_STATE_DONE;
			return NOTIFICATION_STATE_DONE;
		} else {
			CurrentState = NOTIFICATION_STATE_BUSY;	
		}
		
		NotificationTimerRun(active_notification->intervals[notification_idx]);
					
		softSerialPrintInt(active_notification->color);
		softSerialPrint(", ");
		softSerialPrint(INotificationColorToString(active_notification->color));
		softSerialPrintLn("");
		break;

	case NOTIFICATION_STATE_BUSY: {
	
		/* Turn on the LED. */
		RgbLedColorSet(active_notification->color);
		RgbLedModeSet(active_notification->modes[notification_idx]);
	
		notification_idx++;
		if(notification_idx < active_notification->length) {
			/* Set the timer to the notification time. */
			NotificationTimerRun(active_notification->intervals[notification_idx]);
		} else {
			CurrentState = NOTIFICATION_STATE_DONE;
			NotificationTimerRun(NOTIFICATION_STATE_TRANSITION_INTERVAL_MS);
		}
		break;
	}
	case NOTIFICATION_STATE_DONE: {
		/* Turn off the LED. */
		RgbLedModeSet(RGB_LED_MODE_OFF);
		CurrentState = NOTIFICATION_STATE_AWAIT_INPUT;
		return NOTIFICATION_STATE_DONE;
		break;
	}
	
	default: {
		CurrentState = NOTIFICATION_STATE_AWAIT_INPUT;
		break;
	}
	}
	
	
	return CurrentState;
}

static void NotificationTimerRun(uint32_t t_ms)
{
	SoftTimerPeriodSet(NotificationTimer, t_ms);
	TimerFlag = 0;
	SoftTimerStart(NotificationTimer);
}

static void NotificationTimerCallback(SoftTimer_t timer)
{
	SoftTimerStop(NotificationTimer);
	TimerFlag = 1;
}

static NotificationMapping_t *INotificationMappingFromMoistureLevel(MoistureLevel_e level)
{
	uint8_t i = 0;
	uint8_t idx = 0xFF;
	NotificationMapping_t entry;
	
	for(; i < NotificationTableSize; i++) {
		//memcpy_P(&entry, &NotificationMapping[i], sizeof(NotificationMapping_t));
		memcpy(&entry, &NotificationMapping[i], sizeof(NotificationMapping_t));
		if(entry.level == level) {
			idx = i;
			break;
		}
	}
	if(idx == 0xFF) {
		return NULL;
	}
	
	return &NotificationMapping[idx];
}

static const char *INotificationColorToString(RgbLedColor_t color)
{
	switch(color) {
		case RGB_LED_COLOR_RED: {
			return "red";
		}
		case RGB_LED_COLOR_VIOLET: {
			return "violet";
		}
		case RGB_LED_COLOR_GREEN: {
			return "green";
		}
		case RGB_LED_COLOR_AQUA: {
			return "aqua";
		}
		case RGB_LED_COLOR_BLUE: {
			return "blue";
		}
		default: {
			return "unknown";
		}
	}
}
