/*
 * mvp.c
 *
 * Created: 19-05-2020 21:44:09
 *  Author: dorspi
 */ 

static MoistureLevel_e MoistValueToLevel(int16_t value);
static MoistureLevel_e SetMostStableLevel(uint8_t th_score);

static MoistureLevel_e CurrentLevel = MOISTURE_LEVEL_VERY_DRY;

void MvpInit(void)
{
	
}

int MvpMoistValueIn(int16_t moist_val)
{
	
}

int MvpMoistLevelOut(MoistureLevel_e *moist_lvl)
{
	if(moist_lvl == NULL) {
		return -1;
	}
	
	uint8_t i = 0;
	uint8_t th_score = 0; /* TODO: Keep track of th scores for each level, compares scores at the end. */
	MoistureLevel_e prev_level = MOISTURE_LEVEL_VERY_DRY;
	MoistureLevel_e level = MOISTURE_LEVEL_VERY_DRY;
	int16_t value = 0;
	
	while(i < MVP_STORED_SAMPLES) {
		value = NvPartRawRead(i);
		level = MoistValueToLevel(value);
		
		if(level == prev_level) {
			th_score++;
		} else {
			prev_level = level;
			SetMostStableLevel(th_score);
			th_score = 0;
		}
		
		i++;
		
	}
	
	SetMostStableLevel(th_score);
	
	*moist_lvl = CurrentLevel;
}

int MvpThresholdScore(void)
{
	
}

static void SetMostStableLevel(uint8_t th_score, MoistureLevel_e level)
{
	static uint8_t prev_th_score = 0;
	
	if(prev_th_score < th_score) {
		CurrentLevel = level;
		prev_th_score = th_score;
	}
}