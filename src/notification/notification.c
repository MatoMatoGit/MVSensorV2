#include "notification.h"
#include "notification_table.h"

/* App. */
#include "sensor.h"

/* Drivers. */
#include "rgb_led.h"
#include "soft_timer.h"
#include "sleep.h"

/* Serial. */
#include "SoftwareSerial.h"

/* System libs. */
#include <util/delay.h>

/* Standard libs. */
#include <stdint.h>

static void NotificationTimerRun(uint32_t t_ms);
static void NotificationTimerCallback(SoftTimer_t timer);
static NotificationMapping_t *INotificationMappingFromMoistureLevel(MoistureLevel_e level);
static const char *INotificationColorToString(RgbLedColor_t color);

static volatile NotificationState_e CurrentState;
static SoftTimer_t NotificationTimer;
static volatile NotificationSleepCallback_t SleepCallback = NULL;
static volatile uint8_t TimerFlag = 0;
static NotificationMapping_t *NotificationMapping = NULL;

void NotificationInit(NotificationSleepCallback_t sleep_cb)
{
	CurrentState = NOTIFICATION_STATE_AWAIT_INPUT;

	RgbLedModeSet(RGB_LED_MODE_OFF);
	NotificationTimer = SoftTimerCreate(NOTIFICATION_STATE_TRANSITION_INTERVAL_MS, NotificationTimerCallback);
	SleepCallback = sleep_cb;
	TimerFlag = 1;
}

NotificationState_e NotificationRun(int16_t moisture_value, NotificationMapping_t *mapping)
{
	if(TimerFlag == 0 ||  moisture_value < 0 || mapping == NULL) {
		return CurrentState;
	}
	
	static uint8_t notification_idx = 0;
	static NotificationMapping_t *active_notification = NULL;
	
	NotificationMapping = mapping;
	
	switch(CurrentState) {
	
	case NOTIFICATION_STATE_AWAIT_INPUT:
		//softSerialPrintInt(moisture_value);
		//softSerialPrint(", ");
		
		if(moisture_value > 0) {
			/* Map the sensor value to
			 * a color and timing. */
			active_notification = INotificationMappingFromMoistureLevel(moisture_value);
			CurrentState = NOTIFICATION_STATE_BUSY;
					
			//softSerialPrintInt(active_notification->color);
			//softSerialPrint(", ");
			//softSerialPrint(INotificationColorToString(active_notification->color));
			//softSerialPrintLn("");
		}
		break;

	case NOTIFICATION_STATE_BUSY: {
	
		/* Turn on the LED. */
		RgbLedColorSet(active_notification->color);
		RgbLedModeSet(active_notification->modes[notification_idx]);

		/* Set the timer to the notification time and transition to the 'Notify off' state. */
		NotificationTimerRun(active_notification->intervals[notification_idx]);
		
		notification_idx++;
		if(notification_idx > active_notification->length) {
			CurrentState = NOTIFICATION_STATE_DONE;
		}
		break;
	}
	case NOTIFICATION_STATE_DONE: {
		/* Turn off the LED. */
		RgbLedModeSet(RGB_LED_MODE_OFF);
	
		if(SleepCallback != NULL) {
			SleepCallback(0);
		}
		CurrentState = NOTIFICATION_STATE_AWAIT_INPUT;
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
	uint8_t idx = 0;
	
	for(; i < NOTIFICATION_COLOR_TABLE_SIZE; i++) {
		if(NotificationMapping[i].level <= level) {
			idx = i;
		}
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
