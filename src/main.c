#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "SoftwareSerial.h"
#include "moisture.h"
#include "rgb_led.h"
#include "notification.h" 
#include "notification_table.h" 
#include "sensor.h"
#include "oc_timer.h"
#include "gpio_moisture.h"
#include "wdt.h"
#include "sleep.h"
#include "soft_timer.h"
#include "ext_int.h"
#include "adc.h"
#include "sample_log.h"
#include "fw_info.h"
#include "lpf.h"

#define ENABLE_DEMO_MODE			0
#define ENABLE_SAMPLE_LOG			0
#define ENABLE_SAMPLE_LOG_PRINT		0
#define ENABLE_SAMPLE_LOG_INDICATOR	0
#define ENABLE_LOG_CLEAR			0
#define ENABLE_SERIAL_OUTPUT		0
#define ENABLE_TOUCH_BTN			1
#define ENABLE_NOTIFICATION			1
#define ENABLE_SCHED_MEASUREMENT	1
#define ENABLE_UNCOND_MEASUREMENT	0

#define RST_SLEEP_INTERVAL_SEC		21600	/* 6 Hrs. */
#define SLEEP_INTERVAL_SEC			300		/* 5 Mins. */
#define MEASURE_INTERVAL_SEC		7200	/* 2 Hrs. */

#define SOFTSERIAL_TX_DDR	DDRB
#define SOFTSERIAL_TX_PORT	PORTB
#define SOFTSERIAL_TX_PIN	PINB1

#define TOUCH_PAD_DDR		DDRB
#define TOUCH_PAD_PORT		PORTB
#define TOUCH_PAD_PIN		PINB2

#define MVT_VERY_DRY	1950
#define MVT_DRY			500
#define MVT_WET			90

/* avrdude.exe -p t85 -B 25 -c usbasp -U flash:w:"C:\Users\dorspi\Documents\Atmel Studio\7.0\mvsensor_fw\Debug\mvsensor_fw.hex":i */
/* avrdude.exe -p t85 -B 25 -c usbasp -U hfuse:w:0xDF:m */
/* avrdude.exe -p t85 -B 25 -c usbasp -U lfuse:w:0xE2:m */

static volatile bool wdt_wake = false;

static volatile uint32_t TouchCnt = 0;
static ExtInt_t TouchInt = NULL;
static ExtInt_t MoistPulseInt = NULL;

void Demo(void);
void WakeupCallback(void);
void TouchIntCallback(bool state);
void Blink(RgbLedColor_t color);
void BlinkFast(RgbLedColor_t color);
static int16_t MeasureMoisture(void); 
static void SwitchToStandby(uint32_t sleep_time_s);
static void NotifyUser(NotificationType_e type, int16_t moisture_value);
static void SerialGpioInit(void);

int main(void)
{
	int16_t moisture_value = 0;
	static int log_res = 0;
	static uint32_t t_sleep = SLEEP_INTERVAL_SEC;
	static uint32_t measure_interval_rem_s = MEASURE_INTERVAL_SEC;
	
	ExtIntInit();
	
#if ENABLE_SERIAL_OUTPUT==1
	softSerialInit(&DDRB, &PORTB, NULL, SOFTWARE_SERIAL_RX_DISABLED, SOFTSERIAL_TX_PIN);
	softSerialBegin(2400);
	SerialGpioInit();
#endif
	FwInfoInit();
	
#if ENABLE_SERIAL_OUTPUT==1
	FwInfoPrint();
#endif

	SensorInit();
		
#if ENABLE_NOTIFICATION==1
	//SoftTimerInit();
	//NotificationInit();
#endif

	SampleLogInit();
	SleepWakeupCallbackSet(WakeupCallback);

    MoistPulseInt = ExtIntRegister(MOISTURE_OUT_PIN, &PORTB, EXT_INT_PIN_CHANGE, 
		PulseInterruptHandler);
		
#if ENABLE_TOUCH_BTN==1
	TouchInt = ExtIntRegister(TOUCH_PAD_PIN, &TOUCH_PAD_PORT, EXT_INT_PIN_LOW, 
		TouchIntCallback);
	ExtIntDisable();
#endif

#if ENABLE_LOG_CLEAR==1
	SampleLogClear();
#endif

#if ENABLE_SCHED_MEASUREMENT==1
	moisture_value = MeasureMoisture();
#endif
	
#if ENABLE_NOTIFICATION==1
	RgbLedInit();
	NotifyUser(NOTIFICATION_TYPE_STATUS, moisture_value);
#endif
		
	SwitchToStandby(RST_SLEEP_INTERVAL_SEC);
	
    while (1)
    {	

#if ENABLE_DEMO_MODE==1
		Demo();
		continue;
#endif
#if ENABLE_SERIAL_OUTPUT==1
		SerialGpioInit();
#endif

#if ENABLE_SAMPLE_LOG_PRINT==1
		SampleLogPrint();
#endif	

#if	ENABLE_SAMPLE_LOG_INDICATOR==1
		RgbLedInit();
		if(log_res ==  0) {
			Blink(RGB_LED_COLOR_BLUE);
		} else {
			Blink(RGB_LED_COLOR_RED);
		}
#endif
		
		SwitchToStandby(t_sleep);
		
		RgbLedInit();
		
#if ENABLE_SERIAL_OUTPUT==1
		SerialGpioInit();
#endif

		
#if ENABLE_SCHED_MEASUREMENT==1
		if(measure_interval_rem_s >= t_sleep) {
			measure_interval_rem_s -= t_sleep;
		} else {
			measure_interval_rem_s = 0;
		}
		
		if(measure_interval_rem_s == 0) {
			moisture_value = MeasureMoisture();
			moisture_value = Lpf(moisture_value);
			measure_interval_rem_s = MEASURE_INTERVAL_SEC;
#if ENABLE_SAMPLE_LOG==1
			log_res = SampleLogAdd((int16_t)moisture_value);
#endif
		}
#endif

#if ENABLE_UNCOND_MEASUREMENT==1
		moisture_value = MeasureMoisture();
#if ENABLE_SERIAL_OUTPUT==1
		softSerialPrintInt(moisture_value);
		softSerialPrintLn("");
#endif
#endif
		
#if ENABLE_NOTIFICATION==1
		NotifyUser(NOTIFICATION_TYPE_WARNING, moisture_value);
#endif
    }
}

void WakeupCallback(void)
{
	if(wdt_wake == false) {
		wdt_wake = true;
		//MoistureInit();
	}
}

void TouchIntCallback(bool state)
{
	TouchCnt++;
	_delay_ms(200);
}

void Blink(RgbLedColor_t color)
{
	RgbLedColorSet(color);
	RgbLedModeSet(RGB_LED_MODE_ON);
	_delay_ms(500);
	RgbLedModeSet(RGB_LED_MODE_OFF);
	_delay_ms(500);	
}

void BlinkFast(RgbLedColor_t color)
{
	RgbLedColorSet(color);
	RgbLedModeSet(RGB_LED_MODE_ON);
	_delay_ms(200);
	RgbLedModeSet(RGB_LED_MODE_OFF);
	_delay_ms(200);
}

static int16_t MeasureMoisture(void)
{
	int16_t moist_val = 0;
	
	MoistureInit(MoistPulseInt);
	moist_val = SensorValueGet();
	MoistureDeinit();
	
	return moist_val;
}

static void SwitchToStandby(uint32_t sleep_time_s)
{
#if ENABLE_SERIAL_OUTPUT==1
	softSerialPrint("sleep: ");
	softSerialPrintInt(sleep_time_s);
	softSerialPrintLn("sec");
#endif
	
	/* Disable all outputs and set all IOs as inputs. */
	PORTB = 0;
	DDRB &= ~(1 << PINB0) & ~(1 << PINB1) & ~(1 << PINB2) & ~(1 << PINB3) & ~(1 << PINB4);
	
	ExtIntEnable(TouchInt);
	SleepForDuration(sleep_time_s);
	ExtIntDisable();
}

static void NotifyUser(NotificationType_e type, int16_t moisture_value)
{
	NotificationMapping_t *mapping = NULL;
	uint8_t table_size = 0;
	static MoistureLevel_e moisture_level = MOISTURE_LEVEL_MOIST;
	
	if(type == NOTIFICATION_TYPE_WARNING) {
		mapping = WarningNotificationTable;
		table_size = WARNING_NOTIFICATION_TABLE_SIZE;
	} else {
		mapping = StatusNotificationTable;
		table_size = STATUS_NOTIFICATION_TABLE_SIZE;
	}
	
	/* Translate the moisture value to a moisture level.
	 * The moisture level is only changed if the warning is currently not
	 * active, i.e. once the moisture level drops below very dry it can
	 * only be turned off by a touch/reset. */
	//if(moisture_level != MOISTURE_LEVEL_VERY_DRY) {
		if(moisture_value >= MVT_VERY_DRY) {
			moisture_level = MOISTURE_LEVEL_VERY_DRY;
		} else if(moisture_value >= MVT_DRY) {
			moisture_level = MOISTURE_LEVEL_DRY;
		} else if(moisture_value < MVT_DRY &&  moisture_value >= MVT_WET) {
			moisture_level = MOISTURE_LEVEL_MOIST;
		} else if(moisture_value < MVT_WET) {
			moisture_level = MOISTURE_LEVEL_WET;
		}
	//}
	
#if ENABLE_SERIAL_OUTPUT==1
	softSerialPrintLn("");
	softSerialPrintInt(moisture_value);
	softSerialPrintLn("");
	softSerialPrintInt(moisture_level);
#endif
	
	if(type == NOTIFICATION_TYPE_WARNING) {
		if(moisture_level == MOISTURE_LEVEL_VERY_DRY) {
			for(uint8_t i = 0; i < 3; i++) {
				BlinkFast(RGB_LED_COLOR_RED);	
			}
		}
	} else {
		if(moisture_level == MOISTURE_LEVEL_VERY_DRY) {
			Blink(RGB_LED_COLOR_RED);
		} else {
			for(uint8_t i = 0; i < moisture_level; i++) {
				Blink(RGB_LED_COLOR_BLUE);
			}
		}
	}
	/*
	while(NotificationRun(moisture_level, mapping, table_size) != NOTIFICATION_STATE_DONE){
		for(volatile uint32_t i = 0; i < 500; i++);
	}
	*/
}


void Demo(void)
{
	RgbLedInit();
	
	Blink(RGB_LED_COLOR_BLUE);
	Blink(RGB_LED_COLOR_BLUE);
	Blink(RGB_LED_COLOR_BLUE);
	
	_delay_ms(5000);
	
	Blink(RGB_LED_COLOR_BLUE);
	Blink(RGB_LED_COLOR_BLUE);
	
	_delay_ms(5000);

	Blink(RGB_LED_COLOR_BLUE);
	
	_delay_ms(5000);
	
	Blink(RGB_LED_COLOR_RED);
	
	_delay_ms(5000);
	
	BlinkFast(RGB_LED_COLOR_RED);
	BlinkFast(RGB_LED_COLOR_RED);
	BlinkFast(RGB_LED_COLOR_RED);
	
	_delay_ms(5000);
}

static void SerialGpioInit(void)
{
	DDRB |= (1 << SOFTSERIAL_TX_PIN);
	PORTB |= (1 << SOFTSERIAL_TX_PIN);	
}