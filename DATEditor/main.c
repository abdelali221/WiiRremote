/*
    DATEditor v0.3
    Created by Abdelali221
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>

#define VER "v0.3"
#define NUM_OF_PROTOCOLS 6

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
    SAMSUNG48,
    PANASONIC,
    RC5
};

char *PROTOCOLS_NAMES[6] = {
    "NEC",
    "KASEIKYO",
    "LG",
    "SAMSUNG32",
    "SAMSUNG48",
    "PANASONIC",
    "RC5"
};

uint32_t swap_endian_32(uint32_t val) {
    return ((val << 24) & 0xFF000000) |
           ((val << 8)  & 0x00FF0000) |
           ((val >> 8)  & 0x0000FF00) |
           ((val >> 24) & 0x000000FF);
}

int protocolnametonum(char* protocol) {
    for (size_t i = 0; i < NUM_OF_PROTOCOLS; i++) {
        if(!strcmp(protocol, PROTOCOLS_NAMES[i])) return i;
    }    
    return -1;
}
char* numtoprotocolname(int n) {
    if(n < NUM_OF_PROTOCOLS){
        return PROTOCOLS_NAMES[n];
    }
    return "UNKNOWN";
}

void printhelp(const char* filename) {
    printf("\n\n Usage : %s (\"PATH_TO_CODES.DAT\" or -C) Arguments\n", filename);
    printf("\n  -A : Add a code");
    printf("\n  -R : Read the file (use only if a proper path was given)\n");
}

void printaddhelp(const char* filename) {
    printf("\n\n Please follow this structure when adding a code :\n");
    printf(" %s \"FILEPATH\" -A (NAME) (PROTOCOL) (ADDRESS) (CODE)\n", filename);
}

int main(int argc, char *argv[])
{
    printf("\x1b[2J");
    POSCursor(1, 1);
    printf("\n====== WiiRremote codes.dat editor ======");
    printf("\n======== Created by Abdelali221 =========");

    printf("\n\n Ver : %s", VER);

    if(argc < 2) {
        printhelp(argv[0]);
        return -1;
    }
    FILE* code;
    uint32_t numofcodes = 0;
    if(!strcmp(argv[1], "-C")) {
        printf("\n Creating file...");
        code = fopen("codes.dat", "wb");
        fwrite(&numofcodes, 4, 1, code);
        printf("Success!");
        return 0;
    } else {
        printf("\n Opening file...");
        code = fopen(argv[1], "rb+");
    }

    if(!code) {
        printf("Failed!");
        return -1;
    }

    printf("Success!\n");

    if (argc < 3) {
        printf("\n Please provide an operation to be done.");
        printhelp(argv[0]);
        fclose(code);
        return -1;
    }

    if (!strcmp(argv[2], "-A")) {
        if(argc == 7) {
            if(protocolnametonum(argv[4]) == -1) {
                printf("\n Invalid protocol name!");
                printf(" Please use one of the following : \n");
                for (uint8_t i = 0; i < NUM_OF_PROTOCOLS; i++) {
                    printf(" %s,", numtoprotocolname(i));
                }
                printf("\b \n");
                fclose(code);
                return -1;
            }
            if (strlen(argv[3]) > 8) {
                printf(" ERROR! Code name can't exceed 8 characters!");
                fclose(code);
                return -1;
            }
            fseek(code, 0, SEEK_SET);
            fread(&numofcodes, 4, 1, code);
            numofcodes = swap_endian_32(numofcodes);
            printf("\n Adding code :\n");
            IR_data write = {0};
            strcpy(write.name, argv[3]);
            write.protocol = protocolnametonum(argv[4]);
            write.address = strtol(argv[5], NULL, 0);
            write.command = strtol(argv[6], NULL, 0);
            printf("\n Name : %s", write.name);
            printf("\n Protocol : %s", numtoprotocolname(write.protocol));
            printf("\n Address : 0x%X", write.address);
            printf("\n Command : 0x%X", write.command);
            write.address = swap_endian_32(write.address);
            write.command = swap_endian_32(write.command);
            fseek(code, 0, SEEK_END);
            fwrite(&write, 1, sizeof(IR_data), code);
            fseek(code, 0, SEEK_SET);
            numofcodes++;
            numofcodes = swap_endian_32(numofcodes);
            fwrite(&numofcodes, 4, 1, code);
            printf("\n\n Done.");
        } else if (argc < 7) {
            printf("\n Too Few Arguments.");
            printaddhelp(argv[0]);
        } else {
            printf("\n Too Many Arguments.");
            printaddhelp(argv[0]);
        }
    }
    
    if (!strcmp(argv[2], "-R")) {
        printf("\n Reading the file header...");
        fseek(code, 0, SEEK_SET);
        fread(&numofcodes, 4, 1, code);

        numofcodes = swap_endian_32(numofcodes);
        if (strcmp(argv[1], "-C") && numofcodes > 0) {
            printf(" %d codes are available\n", numofcodes);
                IR_data* code_array = calloc(sizeof(IR_data), numofcodes);
            fread(code_array, sizeof(IR_data), numofcodes, code);
            for (size_t i = 0; i < numofcodes; i++)
            {
                printf("\n Code %d", i);
                printf("\n Name : %s", code_array[i].name);
                printf("\n Protocol : %s", numtoprotocolname(code_array[i].protocol));
                printf("\n Address : 0x%X", swap_endian_32(code_array[i].address));
                printf("\n Command : 0x%X\n", swap_endian_32(code_array[i].command));
            } 
        }
        
        if (numofcodes == 0) {
            printf(" There are no codes to be read!");
        }
    }

    fclose(code);
    return 0;
}