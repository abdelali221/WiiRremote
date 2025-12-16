#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>

#include "WiiVT.h"
#include "IR.h"
#include "protocols.h"
#include "TUI.h"

#define VER "v0.3"

const char *str = "Abdelali221\n";

extern void usleep(u32 s);
static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

void printTopbar() {
    POSCursor(0, 0);
    
    printf("%s%*c%s%s%*c", WHITE_BG_BLACK_FG, 35 - ((strlen("WiiRremote ") + strlen(VER)) / 2), ' ', "WiiRremote ", VER, 41 - ((strlen("WiiRremote") + strlen(VER)) / 2), ' ');

    printf("%s", DEFAULT_BG_FG);
}

void PRINT_CODE_INFO(IR_data* IR, u32 currentcode) {
	printTopbar();
	POSCursor(25, 4);
	printf("Using Code %d", currentcode);
	POSCursor(25, 6);
	printf("Name : %s", IR->name);
	POSCursor(25, 7);
	printf("Protocol : %s", ID_TO_PROTOCOL_NAME(IR->protocol));
	POSCursor(25, 8);
	printf("Address : 0x%x", IR->address);
	POSCursor(25, 9);
	printf("Command : 0x%x", IR->command);
}

int main(int argc, char **argv) {

	ENABLE_IR(false);

	VIDEO_Init();

	WPAD_Init();

	rmode = VIDEO_GetPreferredMode(NULL);

	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	console_init(xfb,20,20,rmode->fbWidth-20,rmode->xfbHeight-20,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

	VIDEO_Configure(rmode);

	VIDEO_SetNextFramebuffer(xfb);

	VIDEO_ClearFrameBuffer(rmode, xfb, COLOR_BLACK);

	VIDEO_SetBlack(false);

	VIDEO_Flush();

	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

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
	PRINT_CODE_INFO(&IR_codes[currentcode], currentcode);
	while(1) {
		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) break;

		if ( pressed & WPAD_BUTTON_A ) { 
			GET_PROTOCOL_AND_SEND(&IR_codes[currentcode]);
			POSCursor(30, 10);
			printf("Sent data!");
			usleep(200000);
			ClearScreen();
			PRINT_CODE_INFO(&IR_codes[currentcode], currentcode);
		}

		if ( pressed & WPAD_BUTTON_PLUS ) { 
			if (currentcode < numofcodes - 1) currentcode++;
			ClearScreen();
			PRINT_CODE_INFO(&IR_codes[currentcode], currentcode);
		}

		if ( pressed & WPAD_BUTTON_MINUS ) { 
			if (0 < currentcode) currentcode--;
			ClearScreen();
			PRINT_CODE_INFO(&IR_codes[currentcode], currentcode);
		}
		
	}
	free(IR_codes);
	return 0;
}
