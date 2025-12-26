/*
   IR.c

   Copyright (C) 2025 B. Abdelali.

   This file is part of WiiRremote : https://github.com/abdelali221/WiiRremote.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>

u32 PULSE_TIME = 26399;

extern void usleep(u32 s);
extern void __wiiuse_sensorbar_enable(int enable);

void PWM_IR(bool start, bool end, u32 time, u8 width) {
	struct timespec req = {0};
    struct timespec rem;

	req.tv_nsec = time - ((time*width)/100);
	__wiiuse_sensorbar_enable(start);
	nanosleep(&req, &rem);
	__wiiuse_sensorbar_enable(end);
	req.tv_nsec = time + ((time*width)/100);
	nanosleep(&req, &rem);
}


u16 length_0 = 24;
u16 length_1 = 48;
u16 dutycycle_0 = 50;
u16 dutycycle_1 = 50;
u16 sleep_0 = 0;
u16 sleep_1 = 0;

void SET_PULSE_TIME(u32 t) {
    PULSE_TIME = t;
}

void SET_SETTINGS(u16 l_0, u16 l_1, u16 s_0, u16 s_1, u16 dc_0, u16 dc_1) {
    length_0 = l_0;
    length_1 = l_1;
    sleep_0 = s_0;
    sleep_1 = s_1;
    dutycycle_0 = dc_0;
    dutycycle_1 = dc_1;
}

void SEND_BIT(bool bit) {
	if (bit) {
		for (size_t i = 0; i < length_1; i++)
		{
			PWM_IR(true, false, PULSE_TIME, dutycycle_1);
		}
		usleep(sleep_1);

	} else {
		for (size_t i = 0; i < length_0; i++)
		{
			PWM_IR(true, false, PULSE_TIME, dutycycle_0);
		}
		usleep(sleep_0);
	}
}