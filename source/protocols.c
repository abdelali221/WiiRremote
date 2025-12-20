/*
   protocols.c, based on Arduino-IRremote.
  
   Contains functions for sending multiple IR Protocols
  
   GPL-3.0 License
  
   Copyright (c) 2017-2025 Darryl Smith, Armin Joachimsmeyer
   Copyright (c) 2017-2023 Kristian Lauszus
   Copyright (c) 2025 B. Abdelali
  
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


#include "protocols.h"
#include "IR.h"

char *PROTOCOLS_NAMES[NUM_OF_PROTOCOLS] = {
    "NEC",
    "KASEIKYO",
    "LG",
    "SAMSUNG32",
    "SAMSUNG48",
    "PANASONIC",
    "RC5"
};

extern void usleep(u32 s);

void SEND_NEC(IR_data *IR) {
	SET_SETTINGS(11, 11, 1650, 550, 50, 50);

	for (size_t i = 0; i < 165; i++)
	{
		PWM_IR(true, false, PULSE_TIME, 50);
	}
	usleep(4500);
	if (IR->address < 0x100) {
		for (u8 i = 0; i < 8; i++)
		{
			SEND_BIT(((IR->address >> i) & 1) ? 0 : 1);
		}
		for (u8 i = 0; i < 8; i++)
		{
			SEND_BIT(((IR->address >> i) & 1) ? 1 : 0);
		}
	} else {
		for (u8 i = 0; i < 16; i++)
		{
			SEND_BIT(((IR->address >> i) & 1) ? 0 : 1);
		}
	}

    if (IR->command < 0x100) {
		for (u8 i = 0; i < 8; i++)
		{
			SEND_BIT(((IR->command >> i) & 1) ? 0 : 1);
		}
		for (u8 i = 0; i < 8; i++)
		{
			SEND_BIT(((IR->command >> i) & 1) ? 1 : 0);
		}
	} else {
		for (u8 i = 0; i < 16; i++)
		{
			SEND_BIT(((IR->command >> i) & 1) ? 0 : 1);
		}
	}

	for (size_t i = 0; i < 11; i++)
	{
		PWM_IR(true, false, PULSE_TIME, 50);
	}
	
}

void SEND_SAMSUNG(IR_data *IR, bool _32_48) {
	SET_SETTINGS(11, 11, 1575, 525, 50, 50);

    for (size_t i = 0; i < 82; i++)
	{
		PWM_IR(true, false, PULSE_TIME, 50);
	}
	usleep(4450);
	if(!_32_48) {
		if (IR->address < 0x100) {
			for (u8 i = 0; i < 8; i++)
			{
				SEND_BIT(((IR->address >> i) & 1) ? 0 : 1);
			}
			for (u8 i = 0; i < 8; i++)
			{
				SEND_BIT(((IR->address >> i) & 1) ? 1 : 0);
			}
		} else {
			for (u8 i = 0; i < 16; i++)
			{
				SEND_BIT(((IR->address >> i) & 1) ? 0 : 1);
			}
		}
		if (IR->command < 0x100) {
			for (u8 i = 0; i < 8; i++)
			{
				SEND_BIT(((IR->command >> i) & 1) ? 0 : 1);
			}
			for (u8 i = 0; i < 8; i++)
			{
				SEND_BIT(((IR->command >> i) & 1) ? 1 : 0);
			}
		} else {
			for (u8 i = 0; i < 16; i++)
			{
				SEND_BIT(((IR->command >> i) & 1) ? 0 : 1);
			}
		}
	} else {
		for (u8 i = 0; i < 16; i++)
		{
			SEND_BIT(((IR->address >> i) & 1) ? 0 : 1);
		}

		for (u8 i = 0; i < 32; i++)
		{
			SEND_BIT(((IR->command >> i) & 1) ? 0 : 1);
		}
	}
	for (size_t i = 0; i < 11; i++)
	{
		PWM_IR(true, false, PULSE_TIME, 50);
	}
}

u8 reverse_bits(u8 byte) {
    uint8_t reversed_byte = 0;
    for (int i = 0; i < 8; i++) {
        if ((byte >> i) & 1) {
            reversed_byte |= (1 << (7 - i));
        }
    }
    return reversed_byte;
}

void SEND_KASEIKYO(IR_data *IR, u16 VENDOR_ID) {
    SET_SETTINGS(9, 9, 8 * 3 * 50, 9 * 50, 50, 50);

	u8 addr = IR->address;
	u8 cmd = IR->command;

	u8 tdata[6] = {0};

	tdata[0] = (u8)(VENDOR_ID & 0xFF);
	tdata[1] = (u8)(VENDOR_ID >> 8);
	uint8_t tVendorParity = VENDOR_ID ^ (VENDOR_ID >> 8);
    tVendorParity = (tVendorParity ^ (tVendorParity >> 4)) & 0xF;

	tdata[2] = reverse_bits((u8)(((addr << KASEIKYO_VENDOR_ID_PARITY_BITS) | tVendorParity) && 0xFF));
	tdata[3] = reverse_bits((u8)(((addr << KASEIKYO_VENDOR_ID_PARITY_BITS) | tVendorParity) >> 8));

	tdata[4] = cmd;
	tdata[5] = tdata[4] ^ tdata[2] ^ tdata[3];

	for (size_t i = 0; i < 63; i++)
	{
		PWM_IR(true, false, PULSE_TIME, 50);
	}

	usleep(1700);
	for (size_t j = 0; j < 6; j++) {
		for (size_t i = 0; i < 8; i++)
		{
			SEND_BIT((((tdata[j] >> i) & 1) ? 0 : 1));
		}
	}

	for (size_t i = 0; i < 9; i++)
	{
		PWM_IR(true, false, PULSE_TIME, 50);
	}
}

uint32_t computeLGRawDataAndChecksum(uint8_t aAddress, uint16_t aCommand) {
    uint32_t tRawData = ((uint32_t) aAddress << (LG_COMMAND_BITS + LG_CHECKSUM_BITS)) | ((uint32_t) aCommand << LG_CHECKSUM_BITS);
 
    uint8_t tChecksum = 0;
    uint16_t tTempForChecksum = aCommand;
    for (int i = 0; i < 4; ++i) {
        tChecksum += tTempForChecksum & 0xF;
        tTempForChecksum >>= 4; 
    }
    return (tRawData | (tChecksum & 0xF));
}

void SEND_LG(IR_data *IR) {
	SET_SETTINGS(11, 11, 1575, 525, 50, 50);

	u32 data = computeLGRawDataAndChecksum(IR->address, IR->command);

	for (size_t i = 0; i < 165; i++)
	{
		PWM_IR(true, false, PULSE_TIME, 50);
	}
	usleep(4150);

	for (size_t i = 0; i < 28; i++)
	{
		SEND_BIT(((data >> (27 - i)) & 1) ? 0 : 1);		
	}

	for (size_t i = 0; i < 9; i++)
	{
		PWM_IR(true, false, PULSE_TIME, 50);
	}	
}

void SEND_JVC(IR_data *IR) {
	SET_SETTINGS(11, 11, 1575, 525, 50, 50);

	for (size_t i = 0; i < 165; i++)
	{
		PWM_IR(true, false, PULSE_TIME, 50);
	}

	usleep(4150);

	for (u8 i = 0; i < 8; i++)
	{
		SEND_BIT(((IR->address >> i) & 1) ? 0 : 1);
	}

    for (u8 i = 0; i < 8; i++)
	{
		SEND_BIT(((IR->command >> i) & 1) ? 0 : 1);
	}
	for (size_t i = 0; i < 9; i++)
	{
		PWM_IR(true, false, PULSE_TIME, 50);
	}
}

void GET_PROTOCOL_AND_SEND(IR_data *IR) {
    switch (IR->protocol)
    {
        case NEC:
            SEND_NEC(IR);
        break;

		case PANASONIC:
            SEND_KASEIKYO(IR, PANASONIC_VENDOR_ID_CODE);
        break;

		case KASEIKYO:
            SEND_KASEIKYO(IR, 0);
        break;
		
		case SAMSUNG32:
            SEND_SAMSUNG(IR, 0);
        break;

		case SAMSUNG48:
            SEND_SAMSUNG(IR, 1);
        break;

		case LG:
			SEND_LG(IR);
		break;

		case JVC:
			SEND_JVC(IR);
		break;
    
        default:
        break;
    }
    
}

char* ID_TO_PROTOCOL_NAME(int n) {
	if(n < NUM_OF_PROTOCOLS) {
		return PROTOCOLS_NAMES[n];
	}
	return "UNKNOWN";
}