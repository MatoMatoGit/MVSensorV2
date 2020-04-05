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
static NotificationMapping_t *INotificationMappingFromMoistureValue(uint32_t value);
static const char *INotificationColorToString(RgbLedColor_t color);

static volatile NotificationState_t CurrentState;
static SoftTimer_t NotificationTimer;
static volatile NotificationSleepCallback_t SleepCallback = NULL;
static volatile uint8_t TimerFlag = 0;

void NotificationInit(NotificationSleepCallback_t sleep_cb)
{
	CurrentState = NOTIFICATION_STATE_COLOR_CYCLE;

	RgbLedModeSet(RGB_LED_MODE_OFF);
	NotificationTimer = SoftTimerCreate(NOTIFICATION_STATE_TRANSITION_INTERVAL_MS, NotificationTimerCallback);
	SleepCallback = sleep_cb;
	TimerFlag = 1;
}

NotificationState_t NotificationRun(uint32_t moisture_value)
{
	if(TimerFlag == 0) {
		return CurrentState;
	}
	
#if NOTIFICATION_CONFIG_ENABLE_CALIBRATION==1
	static uint32_t cal_time_start = 0;
#endif

	static uint8_t notification_idx = 0;
	static NotificationMapping_t *notification_mapping = NULL;
	
	switch(CurrentState) {
	case NOTIFICATION_STATE_COLOR_CYCLE: {
			RgbLedModeSet(RGB_LED_MODE_ON);
			for(notification_idx = 0; notification_idx < NOTIFICATION_COLOR_TABLE_SIZE; notification_idx++) {
				RgbLedColorSet(NotificationTable[notification_idx].color);
				_delay_ms(500);
			}
			RgbLedModeSet(RGB_LED_MODE_OFF);
#if NOTIFICATION_CONFIG_ENABLE_CALIBRATION==1
		CurrentState = NOTIFICATION_STATE_DETECT_SOIL;
#else
		CurrentState = NOTIFICATION_STATE_MEASURE;
#endif
		break;
	}
#if NOTIFICATION_CONFIG_ENABLE_CALIBRATION==1
	case NOTIFICATION_STATE_DETECT_SOIL: {
		/* Set the LED color to red and turn it on. */
		RgbLedColorSet(RGB_LED_COLOR_RED);
		RgbLedModeSet(RGB_LED_MODE_ON);
		
		/* Verify that the sensor value is above 
		 * the minimum value, indicating that the senor has 
		 * inserted in the soil. Only then transition to the
		 * calibration state. */
		if(moisture_value >= SENSOR_MIN_VALUE) {
			CurrentState = NOTIFICATION_STATE_MEASURE;
		}
		break;
	}
	case NOTIFICATION_STATE_CALIBRATE: {
		/* If the calibration start time is 0 the calibration cycle
		 * has been initiated. Set the calibration start time
		 * and turn off the LED. The timer is set to the calibration
		 * interval. */
		if(cal_time_start == 0) {
			cal_time_start = SoftTimerTotalTimeGet();
			notification_idx = 0;
			RgbLedModeSet(RGB_LED_MODE_OFF);
			NotificationTimerRun(NOTIFICATION_CALIBRATION_INTERVAL_MS);
		
		/* If the total calibration time has been reached set the next state to notify and
		 * change the timer interval. */
		} else if(SoftTimerTotalTimeGet() >= (cal_time_start + CALIBRATION_PERIOD_S)) {
			CurrentState = NOTIFICATION_STATE_MEASURE;
		}
		
		/* Set the next color for the LED and turn it on. */
		RgbLedColorSet(NotificationTable[notification_idx].color);
		RgbLedModeSet(RGB_LED_MODE_ON);
		
		/* Check if the sensor value is at least the minimum value.
		 * Adjust the 'perfect' moisture value. */
		if(moisture_value >= SENSOR_MIN_VALUE) {
			NotificationTable[MOISTURE_LEVEL_PERFECT].value = moisture_value;
		}
		
		/* Turn off the LED. */
		RgbLedModeSet(RGB_LED_MODE_OFF);
		
		/* Select the next LED color. */
		notification_idx++;
		if(notification_idx >= NOTIFICATION_COLOR_TABLE_SIZE) {
			notification_idx = 0;
		}
		break;
	}
#endif
	case NOTIFICATION_STATE_MEASURE: {
		CurrentState = NOTIFICATION_STATE_NOTIFY_ON;
		break;
	}
	case NOTIFICATION_STATE_NOTIFY_ON: {
		//softSerialPrintInt(moisture_value);
		//softSerialPrint(", ");
		
		/* Map the sensor value to
		 * a color and timing. */
		notification_mapping = INotificationMappingFromMoistureValue(moisture_value);
		
		//softSerialPrintInt(notification_mapping->color);
		//softSerialPrint(", ");
		//softSerialPrint(INotificationColorToString(notification_mapping->color));
		//softSerialPrintLn("");
		
		/* Turn on the LED. */
		RgbLedColorSet(notification_mapping->color);
		RgbLedModeSet(RGB_LED_MODE_ON);
		
		/* Set the timer to the notification time and transition to the 'Notify off' state. */
		NotificationTimerRun(notification_mapping->time_notify);
		CurrentState = NOTIFICATION_STATE_NOTIFY_OFF;
		break;
	}
	case NOTIFICATION_STATE_NOTIFY_OFF: {
		/* Turn off the LED. */
		RgbLedModeSet(RGB_LED_MODE_OFF);
	
		if(SleepCallback != NULL) {
			SleepCallback(notification_mapping->time_sleep);
		}
		CurrentState = NOTIFICATION_STATE_SLEEP;
		break;
	}
	case NOTIFICATION_STATE_SLEEP: {
		//softSerialPrintLn("awake");
		
		CurrentState = NOTIFICATION_STATE_MEASURE;
		break;
	}
	
	default: {
		CurrentState = NOTIFICATION_STATE_MEASURE;
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

static NotificationMapping_t *INotificationMappingFromMoistureValue(uint32_t value)
{
	uint8_t i = 0;
	uint8_t idx = 0;
	
	for(; i < NOTIFICATION_COLOR_TABLE_SIZE; i++) {
		if(NotificationTable[i].value <= value) {
			idx = i;
		}
	}
	
	return &NotificationTable[idx];
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
