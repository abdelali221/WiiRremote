#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>

void POSCursor(uint8_t X, uint8_t Y) {
	printf("\x1b[%d;%dH", Y, X);
}

typedef struct
{
	char name[8];
    uint8_t protocol;
	uint32_t address;
	uint32_t command;
} IR_data;

enum protocols {
    NEC,
    KASEIKYO,
    LG,
    SAMSUNG32,
    SAMSUNG48
};
int protocolnametonum(char* protocol) {
    if(!strcmp(protocol, "NEC")) {
        return NEC;
    } else if(!strcmp(protocol, "KASEIKYO")) {
        return KASEIKYO;
    } else if(!strcmp(protocol, "LG")) {
        return LG;
    } else if(!strcmp(protocol, "SAMSUNG32")) {
        return SAMSUNG32;
    } else if(!strcmp(protocol, "SAMSUNG48")) {
        return SAMSUNG48;
    }
    return -1;
}
char* numtoprotocolname(int n) {
    switch (n)
    {
        case NEC:
            return "NEC";
        break;

        case KASEIKYO:
            return "KASEIKYO";
        break;

        case LG:
            return "LG";
        break;

        case SAMSUNG32:
            return "SAMSUNG32";
        break;

        case SAMSUNG48:
            return "SAMSUNG48";
        break;
        
        default:
            return "UNKNOWN";
        break;
    }
}

void printhelp(const char* filename) {
    printf("\n\nUsage : %s (PATH_TO_CODES.DAT or -C) Arguments\n", filename);
    printf("\n -+ : Add a code (Needs to be followed by -P, -A and -C)");
    printf("\n -R : Read the file (use only if a proper path was given) ");
    printf("\n -P : Protocol (NEC, KASEIKYO, LG, SAMSUNG32, SAMSUNG48)");
    printf("\n -A : Address of IR signal");
    printf("\n -C : Command of IR signal\n");
}

int main(int argc, char *argv[])
{
    printf("\x1b[2J");
    POSCursor(1, 1);
    printf("\n====== WiiRremote codes.dat editor ======");
    printf("\n======== Created by Abdelali221 =========");

    if(argc < 2) {
        printhelp(argv[0]);
        return -1;
    }
    FILE* code;
    uint32_t numofcodes = 0;
    if(!strcmp(argv[1], "-C")) {
        code = fopen("codes.dat", "wb");
        fwrite(&numofcodes, 4, 1, code);
    } else {
        code = fopen(argv[1], "rb");
    }

    if(!code) {
        printf("\n\nFailed to open file!");
    }

    printf("\n\n Reading the file header...");

    fseek(code, 0, SEEK_SET);
    
    fread(&numofcodes, 4, 1, code);

    printf(" %d codes are available", numofcodes);

    if (!strcmp(argv[2], "-+")) {
        if(argc == 9) {
            if(!strcmp(argv[3], "-P") && !strcmp(argv[5], "-A") && !strcmp(argv[7], "-C")) {
                uint8_t protocol = protocolnametonum(argv[4]);
                uint32_t address = strtol(argv[6], NULL, 0);
                uint32_t command = strtol(argv[8], NULL, 0);
                printf("\nProtocol : %s", numtoprotocolname(protocol));
                printf("\nAddress : %x", address);
                printf("\nCommand : %x", command);

            }
        } else if (argc < 9) {
            printf("Too Few Arguments.");
        } else {
            printf("Too Many Arguments.");
        }
    } else if (!strcmp(argv[2], "-R")) {

    }

    return 0;
}