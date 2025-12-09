#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>

#include "WiiVT.h"
#include "IR.h"
#include "protocols.h"

#define VER "v0.1"

const char *str = "Abdelali221\n";

extern void usleep(u32 s);
static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

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

	fseek(code, 0, SEEK_END);
    long size = ftell(code);

	if((size - 4) != numofcodes * sizeof(IR_data)) {
		printf("File size mismatch!!\n Expected %d / Actual %ld", (numofcodes * sizeof(IR_data)) + 4, size);
		fclose(code);
		usleep(2000000);
		exit(0);
	}

	if(numofcodes == 0) {
		printf("No codes are available or file is corrupt!");
		fclose(code);
		exit(0);
	}

	fseek(code, 4, SEEK_SET);

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
