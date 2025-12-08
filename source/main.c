#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/machine/processor.h>
#include <ogc/lwp_watchdog.h>

#define KASEIKYO_VENDOR_ID_PARITY_BITS   4
#define PANASONIC_VENDOR_ID_CODE    0x2002

#define LG_COMMAND_BITS         16
#define LG_CHECKSUM_BITS         4

u32 PULSE_TIME = 26399;

typedef struct
{
	u16 hdr;
	u16 address;
	u16 command;
} IR_data;

const char *str = "Abdelali221\n";


extern void usleep(u32 s);
static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

#define		HW_REG_BASE		0xd800000
#define		HW_GPIO1OUT		(HW_REG_BASE + 0x0e0)

static vu32* const _ipcReg = (u32*)0xCD000000;	

u32 ACR_ReadReg(u32 reg)
{
	return _ipcReg[reg>>2];
}

void ACR_WriteReg(u32 reg,u32 val)
{
	_ipcReg[reg>>2] = val;
}

void toggleirbar(bool enable) {
	u32 val;
	u32 level;

	level = IRQ_Disable();
	val = (ACR_ReadReg(0xc0)&~0x100);
	if(enable) val |= 0x100;
	ACR_WriteReg(0xc0,val);
	IRQ_Restore(level);
}

void pwmir(bool start, bool end, u32 time, u8 width) {
	struct timespec req = {0};
    struct timespec rem;

	req.tv_nsec = time - ((time*width)/100);
	toggleirbar(start);
	nanosleep(&req, &rem);
	toggleirbar(end);
	req.tv_nsec = time + ((time*width)/100);
	nanosleep(&req, &rem);
}

u16 l_0 = 24; //Length
u16 l_1 = 48;
u16 pd_0 = 100; // duty-cycle
u16 pd_1 = 100;
u16 s_0 = 0;
u16 s_1 = 0;

void sendbit(bool bit) {
	if (bit) {
		for (size_t i = 0; i < l_1; i++)
		{
			pwmir(true, false, PULSE_TIME, 50);
		}
		usleep(s_1);

	} else {
		for (size_t i = 0; i < l_0; i++)
		{
			pwmir(true, false, PULSE_TIME, 50);
		}
		usleep(s_0);
	}
}

void sendnec(IR_data *IR) {
	l_0 = 11; // 11 * 50 = 550
	s_0 = 21 * 3 * 25; // 33 * 50 =
	l_1 = 11;
	s_1 = 21 * 25;

	printf("\n ADDR : ");
	for (size_t i = 0; i < 16; i++)
	{
		printf("%d", ((IR->address >> i) & 1));
		
	}
	printf("\n CMD : ");
	for (size_t i = 0; i < 16; i++)
	{
		printf("%d", ((IR->command >> i) & 1));
		
	}
	for (size_t i = 0; i < 166; i++)
	{
		pwmir(true, false, PULSE_TIME, 50);
	}
	usleep(4500);

	for (size_t i = 0; i < 16; i++)
	{
		sendbit((IR->address >> i) & 1);		
	}

	for (size_t i = 0; i < 8; i++)
	{
		sendbit((IR->command >> i) & 1);
	}

	for (size_t i = 0; i < 8; i++)
	{
		sendbit(1 - ((IR->command >> i) & 1));
	}	

	for (size_t i = 0; i < 22; i++)
	{
		pwmir(true, false, PULSE_TIME, 50);
	}
	
}

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
     */
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

	printf("WiiR by Abdelali221");

	IR_data IR1;

	IR1.hdr = PANASONIC_VENDOR_ID_CODE;
	IR1.address = 0x02FE;
	IR1.command = 0xDC23;

	while(1) {
		

		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsHeld(0);

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);

		if ( pressed & WPAD_BUTTON_A ) { 
			sendnec(&IR1);
			printf("Sent data!\n");
			usleep(200000);
		}
		if ( pressed & WPAD_BUTTON_B ) { 
			sendpanasonic(&IR1);
			printf("Sent data!\n");
			usleep(200000);
		}

		if ( pressed & WPAD_BUTTON_PLUS ) { 
			sendlg(&IR1);
			
			printf("Sent data!\n");
			usleep(200000);
		}

		
	}

	return 0;
}
