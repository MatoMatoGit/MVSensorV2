/*
 * filter.h
 *
 * Created: 14-03-2019 09:54:49
 *  Author: dorspi
 */ 


#ifndef FILTER_H_
#define FILTER_H_

#include <stdint.h>

typedef uint32_t FilterDataType_t;

typedef struct {
	uint32_t n;
	FilterDataType_t *buffer;
	
	uint32_t idx_in;
	FilterDataType_t out;
}Filter_t;

int FilterInit(Filter_t *filter);

int FilterIn(Filter_t *filter, FilterDataType_t val);

int FilterOut(Filter_t *filter, FilterDataType_t *val);

#endif /* FILTER_H_ */