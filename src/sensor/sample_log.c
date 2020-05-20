/*
 * sample_log.c
 *
 * Created: 10-05-2020 14:52:31
 *  Author: dorspi
 */ 

#include "sample_log.h"

#include "SoftwareSerial.h"

#include <avr/eeprom.h>
#include <stdbool.h>

#if SAMPLE_LOG_SIZE - 1 > INT16_MAX
#error "sample_log: SAMPLE_LOG_SIZE exceeds maximum of 32766."
#endif

#define I_SAMPLE_LOG_SIZE SAMPLE_LOG_SIZE + 1

int16_t EEMEM SampleLog[I_SAMPLE_LOG_SIZE];
static int16_t SampleLogIndex = -1;

static int16_t ISampleLogFindIndex(void);

void SampleLogInit(void)
{
	SampleLogIndex = ISampleLogFindIndex();
	if(SampleLogIndex == INVALID_VALUE) {
		SampleLogClear();
	}
	
}

int SampleLogAdd(int16_t sample)
{
	if(SampleLogIndex > (SAMPLE_LOG_SIZE - 1) || SampleLogIndex == INVALID_VALUE) {
		return INVALID_VALUE;
	}
	
	eeprom_write_word((uint16_t *)&SampleLog[SampleLogIndex], (uint16_t)sample);
	SampleLogIndex++;
	
	return 0;
}

void SampleLogClear(void)
{
	for(uint16_t i = 0; i < I_SAMPLE_LOG_SIZE; i++) {
		eeprom_write_word((uint16_t *)&SampleLog[i], (uint16_t)INVALID_VALUE);
	}
	SampleLogIndex = 0;
}

void SampleLogPrint(void)
{
	int16_t value = 0;
	
	softSerialPrintLn("---Sample log---");
	softSerialPrint("Number of entries: ");
	softSerialPrintInt((uint32_t)SampleLogIndex);
	softSerialPrintLn("");
	
	if(SampleLogIndex == INVALID_VALUE) {
		return;
	}
	
	for(uint16_t i = 0; i < SampleLogIndex; i++) {
		value = (int16_t)eeprom_read_word((uint16_t *)&SampleLog[i]);
		softSerialPrintInt((uint32_t)value);
		softSerialPrintLn("");
	}
}

static int16_t ISampleLogFindIndex(void)
{
	int16_t index = -1;
	int16_t value = 0;
	bool format_ok = true;
	
	for(uint16_t i = 0; i < I_SAMPLE_LOG_SIZE; i++) {
		value = (int16_t)eeprom_read_word((uint16_t *)&SampleLog[i]);
		
			softSerialPrintInt((uint32_t)value);
			softSerialPrintLn("");
		
		if(index != INVALID_VALUE) {
			softSerialPrintLn("Checking formatting");
			
			if(value != INVALID_VALUE) {
				softSerialPrintLn("Formatting malformed");
				
				format_ok = false;
				break;
			}
		} else {
			softSerialPrintLn("Checking for invalid value");
			
			if(value == INVALID_VALUE) {
				softSerialPrintLn("Found first invalid value");
				index = i;
			}
		}
	}
	
	if(format_ok == false) {
		index = INVALID_VALUE;
	}
	
	softSerialPrintInt((uint32_t)index);
	softSerialPrintLn("");
	return index;
}