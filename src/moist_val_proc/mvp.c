/*
 * mvp.c
 *
 * Created: 19-05-2020 21:44:09
 *  Author: dorspi
 */ 
#include "mvp.h"

#include <stdlib.h>
#include <string.h>


#define MVT_VERY_DRY	2000
#define MVT_DRY			1700
#define MVT_WET			500
#define MVT_VERY_WET	100

static MoistureLevel_e MoistValueToLevel(int16_t value);
static void FindMostStableLevel(uint8_t th_scores[MOISTURE_LEVEL_NUM]);
static void PrintStoredSamples(void);
static void PrintThresholdScores(uint8_t *th_scores);

static MoistureLevel_e CurrentLevel = MOISTURE_LEVEL_VERY_DRY;

static int16_t StoredSamples[MVP_STORED_SAMPLES] = {-1};

void MvpInit(void)
{
	for(uint16_t i = 0; i < MVP_STORED_SAMPLES; i++) {
	    StoredSamples[i] = -1;
	}
}

int MvpMoistValueIn(int16_t moist_val)
{
	if(moist_val < 0) {
	return -1;
	}
    
	uint16_t w_idx = MVP_STORED_SAMPLES + 1;

	for(uint16_t i = 0; i < MVP_STORED_SAMPLES; i++) {
	    if(StoredSamples[i] == -1) {
		w_idx = i;
		break;
	    }
	}

	if(w_idx == MVP_STORED_SAMPLES + 1) {
	    w_idx = MVP_STORED_SAMPLES - 1;
	    
	    int16_t samples_copy[MVP_STORED_SAMPLES] = {0};
	    memcpy(samples_copy, StoredSamples, sizeof(samples_copy));
	    for(uint16_t i = 0; i < MVP_STORED_SAMPLES; i++) {
	    	StoredSamples[i] = samples_copy[i + 1];
	    }
	}

	StoredSamples[w_idx] = moist_val;

	//PrintStoredSamples();

	return 0;
}

int MvpMoistLevelOut(MoistureLevel_e *moist_lvl)
{
	if(moist_lvl == NULL) {
		return -1;
	}
	
	uint16_t i = 0;
	uint8_t th_scores[MOISTURE_LEVEL_NUM] = {0};

	MoistureLevel_e level = MOISTURE_LEVEL_VERY_DRY;
	int16_t value = 0;
	
	while(i < MVP_STORED_SAMPLES) {
		value = StoredSamples[i];
		if(value == -1) {
		    break;
		}
		level = MoistValueToLevel(value);
		th_scores[level]++;
		i++;
	}
	
	//PrintThresholdScores(th_scores);
	
	FindMostStableLevel(th_scores);
	
	*moist_lvl = CurrentLevel;
}

const char *MoistureLevelToString(MoistureLevel_e moist_level)
{
    switch(moist_level) {
    case MOISTURE_LEVEL_VERY_DRY:
        return "Very dry";
    case MOISTURE_LEVEL_DRY:
        return "Dry";
    case MOISTURE_LEVEL_MOIST:
        return "Moist";
    case MOISTURE_LEVEL_WET:
        return "Wet";
    }
}

static MoistureLevel_e MoistValueToLevel(int16_t moisture_value)
{
    
    MoistureLevel_e moisture_level;
    
	if(moisture_value >= MVT_VERY_DRY) {
		moisture_level = MOISTURE_LEVEL_VERY_DRY;

	} else if(moisture_value < MVT_VERY_DRY && moisture_value >= MVT_DRY) {
		moisture_level = MOISTURE_LEVEL_DRY;

	} else if(moisture_value < MVT_DRY && moisture_value >= MVT_WET) {
		moisture_level = MOISTURE_LEVEL_MOIST;

	} else if(moisture_value < MVT_WET && moisture_value >= MVT_VERY_WET) {
		moisture_level = MOISTURE_LEVEL_WET;

	}
	
	return moisture_level;
}

static void FindMostStableLevel(uint8_t th_scores[MOISTURE_LEVEL_NUM])
{
	uint8_t prev_th_score = 0;
	MoistureLevel_e msl = MOISTURE_LEVEL_VERY_DRY;
	
	msl = CurrentLevel;
	for(uint8_t i = 0; i < MOISTURE_LEVEL_NUM; i++) {
		if(prev_th_score < th_scores[i]) {
			msl = (MoistureLevel_e)i;
			prev_th_score = th_scores[i];
		}
	}
	
	CurrentLevel = msl;
}

/*
static void PrintStoredSamples(void)
{
    printf("---Stored samples---\r\n");
    for(uint8_t i = 0; i < MVP_STORED_SAMPLES; i++) {
        printf("%d: %i\r\n", i, StoredSamples[i]);
    }
}

static void PrintThresholdScores(uint8_t *th_scores)
{
    printf("---Threshold scores---\r\n");
    for(uint8_t i = 0; i < MOISTURE_LEVEL_NUM; i++) {
        printf("%d: %i\r\n", i, th_scores[i]);
    }
}
*/

