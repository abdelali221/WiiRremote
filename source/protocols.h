#ifndef _PROTOCOLS_H_
#define _PROTOCOLS_H_

#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>

enum protocols {
    NEC,
    KASEIKYO,
    LG,
    SAMSUNG32,
    SAMSUNG48,
    PANASONIC
};

#define KASEIKYO_VENDOR_ID_PARITY_BITS   4
#define PANASONIC_VENDOR_ID_CODE    0x2002

#define LG_COMMAND_BITS         16
#define LG_CHECKSUM_BITS         4

typedef struct _IR_data
{
	char name[8];
    u8 protocol;
	u32 address;
	u32 command;
} IR_data;

void GET_PROTOCOL_AND_SEND(IR_data IR);
char* ID_TO_PROTOCOL_NAME(int n);

#endif