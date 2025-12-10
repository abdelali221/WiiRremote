#include "protocols.h"
#include "IR.h"

extern void usleep(u32 s);

void SEND_NEC(IR_data IR) {
	setsettings(11, 11, 1575, 525, 50, 50);

	printf(" ADDR : %x", IR.address );
	printf(" CMD : %x", IR.command);

	for (size_t i = 0; i < 165; i++)
	{
		PWM_IR(true, false, PULSE_TIME, 50);
	}
	usleep(4500);
	
	for (u8 i = 0; i < 16; i++)
	{
		SEND_BIT(((IR.address >> i) & 1) ? 0 : 1);
	}

    for (u8 i = 0; i < 16; i++)
	{
		SEND_BIT(((IR.command >> i) & 1) ? 0 : 1);
	}

	for (size_t i = 0; i < 22; i++)
	{
		PWM_IR(true, false, PULSE_TIME, 50);
	}
	
}

void SEND_Samsung(IR_data IR) {
	setsettings(11, 11, 1575, 525, 50, 50);

	printf(" ADDR : %x", IR.address );
	printf(" CMD : %x", IR.command);

    for (size_t i = 0; i < 80; i++)
	{
		PWM_IR(true, false, PULSE_TIME, 50);
	}
	usleep(4500);

    for (u8 i = 0; i < 16; i++)
	{
		SEND_BIT(((IR.address >> i) & 1) ? 0 : 1);
	}

    for (u8 i = 0; i < 16; i++)
	{
		SEND_BIT(((IR.command >> i) & 1) ? 0 : 1);
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

void SEND_KASEIKYO(IR_data IR, u16 VENDOR_ID) {
    setsettings(9, 9, 9*6*25, 9*3*25, 50, 50);

	printf(" ADDR : %x", IR.address);
	printf(" CMD : %x", IR.command);

	u8 addr = IR.address;
	u8 cmd = IR.command;

	u8 tdata[6] = {0};

	tdata[0] = (u8)(PANASONIC_VENDOR_ID_CODE & 0xFF);
	tdata[1] = (u8)(PANASONIC_VENDOR_ID_CODE >> 8);
	uint8_t tVendorParity = PANASONIC_VENDOR_ID_CODE ^ (PANASONIC_VENDOR_ID_CODE>> 8);
    tVendorParity = (tVendorParity ^ (tVendorParity >> 4)) & 0xF;

	tdata[2] = reverse_bits((u8)(((addr << KASEIKYO_VENDOR_ID_PARITY_BITS) | tVendorParity) && 0xFF));
	tdata[3] = reverse_bits((u8)(((addr << KASEIKYO_VENDOR_ID_PARITY_BITS) | tVendorParity) >> 8));

	tdata[4] = cmd;
	tdata[5] = tdata[4] ^ tdata[2] ^ tdata[3];
	for (size_t i = 0; i < 6; i++)
	{
		printf(" %d : %x", i, tdata[i]);
	}
	
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


/*

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


void sendlg(IR_data *IR) {
	l_0 = 11; // 11 * 50 = 550
	s_0 = 21 * 3 * 25; // 33 * 50 =
	l_1 = 11;
	s_1 = 21 * 25;


	u32 data = 0xE0E019E6;

	printf("\n ADDR : ");
	for (size_t i = 0; i < 15; i++)
	{
		printf("%d", ((IR->address >> i) & 1));
		
	}
	printf("\n CMD : ");
	for (size_t i = 0; i < 15; i++)
	{
		printf("%d", ((IR->command >> i) & 1));
		
	}
	for (size_t i = 0; i < 190; i++)
	{
		PWM_IR(true, false, PULSE_TIME, 50);
	}
	
	usleep(4500);

	for (size_t i = 0; i < 28; i++)
	{
		SEND_BIT((data >> (27 - i)) & 1);		
	}

	for (size_t i = 0; i < 22; i++)
	{
		PWM_IR(true, false, PULSE_TIME, 50);
	}
	
}
*/

void GET_PROTOCOL_AND_SEND(IR_data IR) {
    switch (IR.protocol)
    {
        case NEC:
            sendnec(IR);
        break;

		case KASEIKYO:
            sendpanasonic(IR);
        break;
    
        default:
        break;
    }
    
}