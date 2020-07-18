/*
 * lpf.c
 *
 * Created: 14-07-2020 18:49:29
 *  Author: dorspi
 */ 

#include "lpf.h"

#define FILTER_DEPTH 3

static uint32_t x_n[FILTER_DEPTH] = {0};

static uint32_t ShiftFraction(uint32_t x, uint8_t mul, uint8_t div);

uint32_t Lpf(uint32_t x)
{
	uint32_t y = 0;
	
	x_n[2] = x_n[1];
	x_n[1] = x_n[0];
	x_n[0] = x;
	
	y = ShiftFraction(x_n[0], LPF_B0_MUL_SHIFT_0,  LPF_DIV_SHIFT);
	y += ShiftFraction(x_n[0], LPF_B0_MUL_SHIFT_1,  LPF_DIV_SHIFT);
	y += ShiftFraction(x_n[1], LPF_B1_MUL_SHIFT,  LPF_DIV_SHIFT);
	y += ShiftFraction(x_n[2], LPF_B2_MUL_SHIFT,  LPF_DIV_SHIFT);
	
	return y;
}

static uint32_t ShiftFraction(uint32_t x, uint8_t mul, uint8_t div)
{
	x = x << mul;
	x = x >> div;
	return x;
}