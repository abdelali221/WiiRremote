#include "protocols.h"
#include "IR.h"

extern void usleep(u32 s);

void sendnec(IR_data IR) {
	setsettings(11, 11, 1575, 525, 50, 50);

	printf(" ADDR : %x", IR.address );
	printf(" CMD : %x", IR.command);

	for (size_t i = 0; i < 165; i++)
	{
		pwmir(true, false, PULSE_TIME, 50);
	}
	usleep(4500);
	
	for (u8 i = 0; i < 16; i++)
	{
		sendbit(((IR.command >> i) & 1) ? 0 : 1);
	}

	for (u8 i = 0; i < 16; i++)
	{
		sendbit(((IR.address >> i) & 1) ? 0 : 1);
	}

	for (size_t i = 0; i < 22; i++)
	{
		pwmir(true, false, PULSE_TIME, 50);
	}
	
}

void sendpanasonic(IR_data *IR) {
    setsettings(9, 9, 18*25, 9*25, 50, 50);

	u8 tdata[6] = {0};

	tdata[0] = (u8)(PANASONIC_VENDOR_ID_CODE & 0xFF);
	tdata[1] = (u8)(PANASONIC_VENDOR_ID_CODE >> 8);
	uint8_t tVendorParity = PANASONIC_VENDOR_ID_CODE ^ (PANASONIC_VENDOR_ID_CODE>> 8);
    tVendorParity = (tVendorParity ^ (tVendorParity >> 4)) & 0xF;

	tdata[2] = (u8)(((IR->address << KASEIKYO_VENDOR_ID_PARITY_BITS) | tVendorParity) && 0xFF);
	tdata[3] = (u8)(((IR->address << KASEIKYO_VENDOR_ID_PARITY_BITS) | tVendorParity) >> 8);

	tdata[4] = (u8)IR->command;
	tdata[5] = tdata[4] ^ tdata[2] ^ tdata[3];

	printf("Binary Sequence : \n");
	printf(" tdata : ");
	for (size_t j = 0; j < 5; j++)
	{
		for (size_t i = 0; i < 8; i++)
		{
			printf("%d", ((tdata[j] >> i) & 1));
		}
		printf("\n         ");
	}

	for (size_t i = 0; i < 69; i++)
	{
		pwmir(true, false, PULSE_TIME, 50);
	}

	usleep(2400);
	for (size_t j = 0; j < 6; j++) {
		for (size_t i = 0; i < 8; i++)
		{
			sendbit((((tdata[j] >> i) & 1) ? 0 : 1));
		}
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
		pwmir(true, false, PULSE_TIME, 50);
	}
	
	usleep(4500);

	for (size_t i = 0; i < 28; i++)
	{
		sendbit((data >> (27 - i)) & 1);		
	}

	for (size_t i = 0; i < 22; i++)
	{
		pwmir(true, false, PULSE_TIME, 50);
	}
	
}
*/

void getprotocolandsend(IR_data IR) {
    switch (IR.protocol)
    {
        case NEC:
            sendnec(IR);
        break;
    
        default:
        break;
    }
    
}