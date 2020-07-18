/*
 * sample_log.h
 *
 * Created: 10-05-2020 14:50:51
 *  Author: dorspi
 */ 


#ifndef SAMPLE_LOG_H_
#define SAMPLE_LOG_H_

#include <stdint.h>

#define SAMPLE_LOG_SIZE 0

#define INVALID_VALUE -1

void SampleLogInit(void);

int SampleLogAdd(int16_t sample);

void SampleLogClear(void);

void SampleLogPrint(void);


#endif /* SAMPLE_LOG_H_ */