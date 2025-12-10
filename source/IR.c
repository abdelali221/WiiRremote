#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>

extern void usleep(u32 s);

static vu32* const _ipcReg = (u32*)0xCD000000;	

u32 PULSE_TIME = 26399;

u32 ACR_ReadReg(u32 reg)
{
	return _ipcReg[reg>>2];
}

void ACR_WriteReg(u32 reg,u32 val)
{
	_ipcReg[reg>>2] = val;
}

void ENABLE_IR(bool enable) {
	u32 val;
	u32 level;

	level = IRQ_Disable();
	val = (ACR_ReadReg(0xc0)&~0x100);
	if(enable) val |= 0x100;
	ACR_WriteReg(0xc0,val);
	IRQ_Restore(level);
}

void PWM_IR(bool start, bool end, u32 time, u8 width) {
	struct timespec req = {0};
    struct timespec rem;

	req.tv_nsec = time - ((time*width)/100);
	ENABLE_IR(start);
	nanosleep(&req, &rem);
	ENABLE_IR(end);
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