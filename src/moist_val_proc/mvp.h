/*
 * mvp.h
 *
 * Moisture Value Processing library.
 * This library takes raw moisture values as input and
 * applies processing to them. The output is a moisture
 * level.
 * 
 * Current processing implementation
 * 1. Store input value(s), up to MVP_STORED_SAMPLES. The oldest
 * sample is discarded when the stored amount exceeds the maximum.
 * 2. Compare the stored samples to pre-defined moisture value
 * thresholds. 
 * 3. Determine how many consecutive samples are below or above a
 * certain threshold.
 * 4. Determine the  moisture level from the threshold evaluation.
 *  
 * Created: 19-05-2020 21:43:58
 *  Author: dorspi
 */ 


#ifndef MVP_H_
#define MVP_H_

#include "moisture_level.h"

#include <stdint.h>

#define MVP_STORED_SAMPLES 5

#define MVP_CONSECUTIVE_SAMPLES 5

void MvpInit(void);

int MvpMoistValueIn(int16_t moist_val);

int MvpMoistLevelOut(MoistureLevel_e *moist_lvl);

const char *MoistureLevelToString(MoistureLevel_e moist_level);



#endif /* MVP_H_ */
