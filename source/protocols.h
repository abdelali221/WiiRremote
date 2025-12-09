#ifndef _PROTOCOLS_H_
#define _PROTOCOLS_H_

#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>


#define KASEIKYO_VENDOR_ID_PARITY_BITS   4
#define PANASONIC_VENDOR_ID_CODE    0x2002

#define LG_COMMAND_BITS         16
#define LG_CHECKSUM_BITS         4

typedef struct
{
	char hdr[8];
	vu16 address;
	vu16 command;
} IR_data;

void sendnec(IR_data IR);

#endif