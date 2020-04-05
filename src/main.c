#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

#define SOFTSERIAL_TX_DDR	DDRB
#define SOFTSERIAL_TX_PORT	PORTB
#define SOFTSERIAL_TX_PIN	PINB1

/* avrdude.exe -p t85 -B 25 -c U flash:w:"C:\Users\dorspi\Documents\Atmel Studio\7.0\mvsensor_fw\Debug\mvsensor_fw.hex":i */

static volatile uint8_t woken = 1;
static volatile uint32_t t_sleep = 0;

void SleepCallback(uint32_t duration_s);
void WakeupCallback(void);
void TouchIntCallback(bool state);

int main(void)
{
	uint32_t moisture_value = 0;
	//DDRB |= (1 << SOFTSERIAL_TX_PIN);
	//PORTB &= ~(1 << PINB2);
	//PORTB |= (1 << SOFTSERIAL_TX_PIN);

	/* Initialize drivers. */
	SoftTimerInit();
	MoistureInit();
	RgbLedInit();
	ExtIntInit();
	//softSerialInit(&DDRB, &PORTB, NULL, SOFTWARE_SERIAL_RX_DISABLED, SOFTSERIAL_TX_PIN);
	//softSerialBegin(2400);

	/* Initialize app components. */
	SensorInit();
	NotificationInit(SleepCallback);
	SleepWakeupCallbackSet(WakeupCallback);

    ExtInt_t touch_int = ExtIntRegister(PINB0, &PORTB, EXT_INT_PIN_RISING, TouchIntCallback);
    ExtIntEnable(touch_int);

    while (1)
    {
		if(woken == 1) {
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
    }
}

void SleepCallback(uint32_t duration_s)
{
	MoistureDeinit();
	t_sleep = duration_s;
	woken = 0;
}

void WakeupCallback(void)
{
	if(woken == 0) {
		woken = 1;
		MoistureInit();
	}
}

void TouchIntCallback(bool state)
{

}
