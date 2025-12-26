#ifndef _IR_H_
#define _IR_H_
#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>

extern u32 PULSE_TIME;

void SET_SETTINGS(u16 l_0, u16 l_1, u16 s_0, u16 s_1, u16 dc_0, u16 dc_1);
void SET_PULSE_TIME(u32 t);
void SEND_BIT(bool bit);
void PWM_IR(bool start, bool end, u32 time, u8 width);

#endif