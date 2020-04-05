/*
 * filter.c
 *
 * Created: 14-03-2019 09:58:58
 *  Author: dorspi
 */ 

#include "filter.h"

#include <stdlib.h>
#include <string.h> /* For memset. */

static void IAdvanceIndex(Filter_t *filter);

int FilterInit(Filter_t *filter)
{
	if(filter == NULL) {
		return -1;
	}
	if(filter->buffer == NULL || filter->n == 0) {
		return -2;
	}
	
	memset(filter->buffer, 0, filter->n * sizeof(FilterDataType_t));
	filter->idx_in = 0;
	filter->out = 0;
	
	return 0;
}


int FilterIn(Filter_t *filter, FilterDataType_t val)
{
	if(filter == NULL) {
		return -1;
	}
		
	/* Store the new sample in the buffer */
	filter->buffer[filter->idx_in] = val;
	
	filter->out = 0;
	for(uint32_t i = 0; i < filter->n; i++) {
		filter->out += filter->buffer[i];
	}

	/* Calculate the new output. */
	filter->out = filter->out / filter->n;
	
	/* Advance idx_in for the next sample. */
	IAdvanceIndex(filter);
	
	return 0;
}

int FilterOut(Filter_t *filter, FilterDataType_t *val)
{
	if(filter == NULL || val == NULL) {
		return -1;
	}
	
	*val = filter->out;
	
	return 0;
}

static void IAdvanceIndex(Filter_t *filter)
{
	filter->idx_in++;
	if(filter->idx_in >= filter->n) {
		filter->idx_in = 0;
	}
}