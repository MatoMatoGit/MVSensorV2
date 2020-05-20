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
#include "sensor.h"
#include "oc_timer.h"
#include "gpio_moisture.h"
#include "wdt.h"
#include "sleep.h"
#include "soft_timer.h"
#include "ext_int.h"
#include "adc.h"
#include "sample_log.h"

#define SOFTSERIAL_TX_DDR	DDRB
#define SOFTSERIAL_TX_PORT	PORTB
#define SOFTSERIAL_TX_PIN	PINB1

/* avrdude.exe -p t85 -B 25 -c usbasp -U flash:w:"C:\Users\dorspi\Documents\Atmel Studio\7.0\mvsensor_fw\Debug\mvsensor_fw.hex":i */

static volatile bool wdt_wake = false;
static volatile bool pin_wake;
static volatile uint8_t pin_state = 0;
static volatile uint32_t t_sleep = 0;
static volatile uint32_t touch_cnt = 0;
static volatile ExtInt_t touch_int  = NULL;

void SleepCallback(uint32_t duration_s);
void WakeupCallback(void);
void TouchIntCallback(bool state);
void Blink(RgbLedColor_t color, uint32_t ms);

int main(void)
{
	//DDRB &= ~(1 << PINB2);
	
	uint32_t moisture_value = 0;
	//DDRB |= (1 << SOFTSERIAL_TX_PIN);
	
	/* Initialize drivers. */
	//SoftTimerInit();
	RgbLedInit();
	ExtIntInit();
	//softSerialInit(&DDRB, &PORTB, NULL, SOFTWARE_SERIAL_RX_DISABLED, SOFTSERIAL_TX_PIN);
	//softSerialBegin(2400);

	/* Initialize app components. */
	SensorInit();
	//NotificationInit(SleepCallback);
	SampleLogInit();
	SleepWakeupCallbackSet(WakeupCallback);
	
	//SampleLogPrint();

    ExtInt_t moist_pulse_int = ExtIntRegister(MOISTURE_OUT_PIN, &PORTB, EXT_INT_PIN_CHANGE, PulseInterruptHandler);
	MoistureInit(moist_pulse_int);
	
	//touch_int = ExtIntRegister(PINB0, &PORTB, EXT_INT_PIN_CHANGE, TouchIntCallback);
	t_sleep = 60;
	
	static uint8_t sleep_cnt = 0;
	static int log_res = 0;
    while (1)
    {	
		
		//DDRB |= (1 << SOFTSERIAL_TX_PIN);
		
		MoistureInit(moist_pulse_int);
		
		//PORTB |= (1 << SOFTSERIAL_TX_PIN);
		//touch_cnt = 0;

		
		if(sleep_cnt > 120) {
			moisture_value = SensorValueGet();
			log_res = SampleLogAdd((int16_t)moisture_value);
			sleep_cnt = 0;
		}
		
		if(log_res != -1) {
			Blink(RGB_LED_COLOR_BLUE, 1000);
		} else {
			Blink(RGB_LED_COLOR_RED, 1000);
		}
		
		sleep_cnt++;		
		
		//softSerialPrintInt(moisture_value);
		//softSerialPrintLn("");
		
		
		//softSerialPrint("sleep: ");
		//softSerialPrintInt(t_sleep);
		//softSerialPrintLn("sec");
		
		ExtIntDisable();
		MoistureDeinit();
		AdcDeinit();
		//PORTB &= ~(1 << SOFTSERIAL_TX_PIN);
		SleepForDuration(t_sleep);
		
		/*
		if(pin_wake == true) {

		}
		
		if(wdt_wake == true) {
			softSerialPrintLn("reason: scheduled");
			
			Blink(25000);
			wdt_wake = false;
		} else {
			softSerialPrintLn("reason: touch/rst");
			
			SampleLogPrint();
			
			ExtIntUnregister(touch_int);
			touch_int = ExtIntRegister(PINB2, &PORTB, EXT_INT_PIN_CHANGE, TouchIntCallback);
			ExtIntEnable(touch_int);
			PORTB &= ~(1 << PINB4);
			for(volatile uint32_t i = 0; i < 400000; i++);
			ExtIntUnregister(touch_int);
			//softSerialPrintInt(touch_cnt);
			for(volatile uint32_t i = 0; i < touch_cnt; i++) {
				Blink(10000);
			}
			PORTB &= ~(1 << PINB4);
			
			pin_wake = false;
		}
		
		//ExtIntUnregister(touch_int);
		touch_int = ExtIntRegister(PINB2, &PORTB, EXT_INT_PIN_FALLING, TouchIntCallback);
		ExtIntEnable(touch_int);
		softSerialPrint("sleep: ");
		softSerialPrintInt(t_sleep);
		softSerialPrintLn("sec");
		SleepForDuration(t_sleep);
		
		*/
		//ExtIntDisable();
		
		//PORTB |= (1 << PINB3);
		//softSerialPrintLn("running");
		//for(volatile int i = 0; i < 30000; i++);
		//moisture_value = SensorValueGet();
		//SampleLogAdd((int16_t)moisture_value);
		//for(uint32_t i = 0; i < moisture_value; i++) {
			//PORTB ^= (1 << PINB3);
		//}
		//PORTB &= ~(1 << PINB3);
		//for(volatile int i = 0; i < 30000; i++);
    /*
		if(wdt_wake == 1) {
			switch(NotificationRun(moisture_value)) {
				case NOTIFICATION_STATE_MEASURE: {
					moisture_value = SensorValueGet();
					break;
				}
				case NOTIFICATION_STATE_SLEEP: {
					SleepForDuration(t_sleep);
					break;
				}
				default: {
					break;
				}
			}
		} else {
			return 0;
		}
		*/
    }
}

void SleepCallback(uint32_t duration_s)
{
	//MoistureDeinit();
	t_sleep = duration_s;
	wdt_wake = false;
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
	touch_cnt++;
	
	pin_wake = true;
	
   // softSerialPrintLn("touch");
    //softSerialPrintInt((uint32_t)state);
	//PORTB |= (1 << PINB4);
	
	static uint8_t led_state = 0;
	
	if(led_state) {
		RgbLedModeSet(RGB_LED_MODE_OFF);
		led_state = 0;
		_delay_ms(100);
	} else {
		RgbLedModeSet(RGB_LED_MODE_ON);
		led_state = 1;
		_delay_ms(100);
	}
}

void Blink(RgbLedColor_t color, uint32_t ms)
{
	RgbLedColorSet(color);
	RgbLedModeSet(RGB_LED_MODE_ON);
	_delay_ms(1000);
	RgbLedModeSet(RGB_LED_MODE_OFF);
	_delay_ms(1000);	
}