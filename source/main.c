#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>

#include "WiiVT.h"
#include "IR.h"
#include "protocols.h"

#define VER "v0.1"

#define KASEIKYO_VENDOR_ID_PARITY_BITS   4
#define PANASONIC_VENDOR_ID_CODE    0x2002

#define LG_COMMAND_BITS         16
#define LG_CHECKSUM_BITS         4


const char *str = "Abdelali221\n";

extern void usleep(u32 s);
static void *xfb = NULL;
static GXRModeObj *rmode = NULL;


/*
void sendpanasonic(IR_data *IR) {
	l_0 = 9;
	s_0 = 9 * 2 * 25;
	l_1 = 9;
	s_1 = 9 * 25;

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
			sendbit(((tdata[j] >> i) & 1));
		}
	}
}

uint32_t computeLGRawDataAndChecksum(uint8_t aAddress, uint16_t aCommand) {
    uint32_t tRawData = ((uint32_t) aAddress << (LG_COMMAND_BITS + LG_CHECKSUM_BITS)) | ((uint32_t) aCommand << LG_CHECKSUM_BITS);
    /*
     * My guess of the 4 bit checksum
     * Addition of all 4 nibbles of the 16 bit command
     *//*
    uint8_t tChecksum = 0;
    uint16_t tTempForChecksum = aCommand;
    for (int i = 0; i < 4; ++i) {
        tChecksum += tTempForChecksum & 0xF; // add low nibble
        tTempForChecksum >>= 4; // shift by a nibble
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

void sendrs232(u8 d) {
	l_1 = 1333;
	s_1 = 0;
	for (size_t i = 0; i < 1333; i++)
	{
		pwmir(true, false, PULSE_TIME, 50);
	}
	for (size_t i = 0; i < 8; i++)
	{
		sendbit((d >> i) & 1);
	}
	for (size_t i = 0; i < 1333; i++)
	{
		pwmir(false, false, PULSE_TIME, 50);
	}

}
*/
//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	// Initialise the video system
	VIDEO_Init();

	// This function initialises the attached controllers
	WPAD_Init();

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth-20,rmode->xfbHeight-20,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	//SYS_STDIO_Report(true);

	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);

	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);

	// Clear the framebuffer
	VIDEO_ClearFrameBuffer(rmode, xfb, COLOR_BLACK);

	// Make the display visible
	VIDEO_SetBlack(false);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();


	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	printf("\x1b[2;0H");

	printf(" WiiRremote %s\n Created by Abdelali221", VER);

	printf("\n Initializing storage device...");

	fatInitDefault();

	printf("Done.");

	printf("\n Opening code file...");

	FILE* code = fopen("codes.dat", "rb");

	if(!code) {
		printf("FAILED!\n Make sure you have it in the /app/WiiRremote/ folder");
		exit(0);
	}

	printf("Done.");

	printf("\n Reading the file header...");

	u32 numofcodes = 0;

	fread(&numofcodes, 4, 1, code);

	if(numofcodes == 0) {
		printf("No codes are available or file is corrupt!");
		fclose(code);
		exit(0);
	}

	printf("%d codes are available.", numofcodes);

	IR_data* IR_codes = calloc(numofcodes, sizeof(IR_data));

	fread(IR_codes, sizeof(IR_data), numofcodes, code);

	fclose(code);

	usleep(2000000);
	ClearScreen();
	u32 currentcode = 0;
	POSCursor(25, 4);
	printf("Using Code %d", currentcode);
	while(1) {
		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) break;

		if ( pressed & WPAD_BUTTON_A ) { 
			POSCursor(20, 6);
			sendnec(IR_codes[currentcode]);
			POSCursor(20, 7);
			printf("Sent data! %d", currentcode);
			usleep(1000000);
			ClearScreen();
			POSCursor(25, 4);
			printf("Using Code %d ", currentcode);
		}

		if ( pressed & WPAD_BUTTON_PLUS ) { 
			if (currentcode < numofcodes - 1) currentcode++;
			POSCursor(25, 4);
			printf("Using Code %d ", currentcode);
		}

		if ( pressed & WPAD_BUTTON_MINUS ) { 
			if (0 < currentcode) currentcode--;
			POSCursor(25, 4);
			printf("Using Code %d ", currentcode);
		}
		
	}
	free(IR_codes);
	return 0;
}
