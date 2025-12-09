/*
    DATEditor v0.1
    Created by Abdelali221
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>

#define VER "v0.1"

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

uint32_t swap_endian_32(uint32_t val) {
    return ((val << 24) & 0xFF000000) |
           ((val << 8)  & 0x00FF0000) |
           ((val >> 8)  & 0x0000FF00) |
           ((val >> 24) & 0x000000FF);
}

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
    printf("\n\nUsage : %s (""PATH_TO_CODES.DAT"" or -c) Arguments\n", filename);
    printf("\n -+ : Add a code (Needs to be followed by -N, -P, -A and -C)");
    printf("\n -R : Read the file (use only if a proper path was given) ");
    printf("\n -N : Name of the code");
    printf("\n -P : Protocol (NEC, KASEIKYO, LG, SAMSUNG32, SAMSUNG48)");
    printf("\n -A : Address of IR signal");
    printf("\n -C : Command of IR signal\n");
}

void printaddhelp(const char* filename) {
    printf("\nPlease follow the following example when adding a code :\n");
    printf("%s codes.dat -+ -N POWER -P NEC -A 0xFFFF -C 0xFFFF", filename);
}

int main(int argc, char *argv[])
{
    printf("\x1b[2J");
    POSCursor(1, 1);
    printf("\n====== WiiRremote codes.dat editor ======");
    printf("\n======== Created by Abdelali221 =========");

    printf("\n\nVer : %s", VER);

    if(argc < 2) {
        printhelp(argv[0]);
        return -1;
    }
    FILE* code;
    uint32_t numofcodes = 0;
    if(!strcmp(argv[1], "-c")) {
        printf("\nCreating file...");
        code = fopen("codes.dat", "wb");
        fwrite(&numofcodes, 4, 1, code);
        printf("Success!");
        return 0;
    } else {
        printf("\nOpening file...");
        code = fopen(argv[1], "rb+");
    }

    if(!code) {
        printf("Failed!");
        return -1;
    }

    printf("Success!");

    printf("\n\n Reading the file header...");
    fseek(code, 0, SEEK_SET);
    fread(&numofcodes, 4, 1, code);

    numofcodes = swap_endian_32(numofcodes);
    printf(" %d codes are available", numofcodes);

    if (!strcmp(argv[2], "-+")) {
        if(argc == 11) {
            if(!strcmp(argv[3], "-N") && !strcmp(argv[5], "-P") && !strcmp(argv[7], "-A") && !strcmp(argv[9], "-C")) {
                IR_data write = {0};
                strcpy(write.name, argv[4]);
                write.protocol = protocolnametonum(argv[6]);
                write.address = strtol(argv[8], NULL, 0);
                write.command = strtol(argv[10], NULL, 0);
                printf("\n Name : %s", write.name);
                printf("\n Protocol : %s", numtoprotocolname(write.protocol));
                printf("\n Address : %x", write.address);
                printf("\n Command : %x", write.command);
                write.address = swap_endian_32(write.address);
                write.command = swap_endian_32(write.command);
                fseek(code, 0, SEEK_END);
                size_t wrote = fwrite(&write, 1, sizeof(IR_data), code);
                if(wrote != sizeof(write)) {
                    printf("\nERROR! Wrote %d Expected %d\n", wrote, sizeof(write));
                }
                fseek(code, 0, SEEK_SET);
                numofcodes++;
                numofcodes = swap_endian_32(numofcodes);
                fwrite(&numofcodes, 4, 1, code);
                printf("\n Done.");
            }
        } else if (argc < 11) {
            printf("Too Few Arguments.");
            printaddhelp(argv[0]);
        } else {
            printf("Too Many Arguments.");
            printaddhelp(argv[0]);
        }
    } else if (!strcmp(argv[2], "-R") && numofcodes > 0) {
        IR_data* code_array = calloc(sizeof(IR_data), numofcodes);
        fread(code_array, sizeof(IR_data), numofcodes, code);
        for (size_t i = 0; i < numofcodes; i++)
        {
            printf("\n Code %d", i);
            printf("\n Name : %s", code_array[i].name);
            printf("\n Protocol : %s", numtoprotocolname(code_array[i].protocol));
            printf("\n Address : %x", swap_endian_32(code_array[i].address));
            printf("\n Command : %x\n", swap_endian_32(code_array[i].command));
        }        
    }
    fclose(code);
    return 0;
}