/*
 * lpf.h
 *
 * Created: 14-07-2020 18:49:12
 *  Author: dorspi
 */ 


#ifndef LPF_H_
#define LPF_H_

#include <stdint.h>

/* y[n] = 8/16 * x[n] + 4/16 * x[n] + 2/16 * x[n-1] + 2/16 * x[n-2] */

#define LPF_DIV_SHIFT 4 /* /16 */

#define LPF_B0_MUL_SHIFT_0	3 /* x8 */
#define LPF_B0_MUL_SHIFT_1	2 /* x4 */
#define LPF_B1_MUL_SHIFT	1 /* x2 */
#define LPF_B2_MUL_SHIFT	1 /* x2 */


/* Returns y[n] */
uint32_t Lpf(uint32_t x);

#endif /* LPF_H_ */