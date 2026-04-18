#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * S80C (Simple 8080 Compiler (Assembler)
 * Version: prototype 1
 * 
 * This is the first version os 8080 assembler, i dont have any idea of what im doing
 * if u are reading this, congratulations for me, my git works
 * */

int IsNumber(char* s) {
	if(*s == '\0' || s == NULL) return 1;
	while(*s) {
		if((unsigned char)*s < '0' || (unsigned char)*s > '9') return 1;
		s++;
	}
	return 0;
}

void GetParam(FILE *f, char *dest) {
    int index = 0;
    int c;
    
    while((c = fgetc(f)) != EOF && (isspace(c) || c == ','));
    
    if(c != EOF) {
        dest[index++] = c;
        while((c = fgetc(f)) != EOF && c != ',' && !isspace(c)) {
            if(index < 127) dest[index++] = c;
        }
    }
    dest[index] = '\0';
}

int GetRegIndex(char* r) {
    if(strcmp(r, "B") == 0) return 0;
    if(strcmp(r, "C") == 0) return 1;
    if(strcmp(r, "D") == 0) return 2;
    if(strcmp(r, "E") == 0) return 3;
    if(strcmp(r, "H") == 0) return 4;
    if(strcmp(r, "L") == 0) return 5;
    if(strcmp(r, "M") == 0) return 6;
    if(strcmp(r, "A") == 0) return 7;
    return -1; // Error
}

int main(int argc, char* argv[]) {
	char inst[128] = {0};
	char param[128] = {0};
	char param1[128] = {0};
	int c;
	int index = 0;
	int pc = 0;
	
	unsigned char exit_code[128] = {0};
	
	FILE *as = fopen(argv[1], "r");
	FILE *bin = fopen("binary.bin", "wb");
	if(as == NULL || bin == NULL) {
		perror("Error:");
		exit(1);
	}
	
	while((c = fgetc(as)) != EOF) {
		if(isspace(c)) {
			if(index > 0) {
				inst[index] = '\0';
				if(strcmp(inst, "MIV") == 0) {
					GetParam(as, param);
					GetParam(as, param1);
					if(strcmp(param, "A") == 0) exit_code[pc++] = 0x3E;
					else if(strcmp(param, "B") == 0) exit_code[pc++] = 0x06;
					else if(strcmp(param, "C") == 0) exit_code[pc++] = 0x0E;
					else if(strcmp(param, "D") == 0) exit_code[pc++] = 0x16;
					else if(strcmp(param, "E") == 0) exit_code[pc++] = 0x1E;
					else if(strcmp(param, "H") == 0) exit_code[pc++] = 0x26;
					else if(strcmp(param, "L") == 0) exit_code[pc++] = 0x2E;
					else if(strcmp(param, "M") == 0) exit_code[pc++] = 0x36;
					else {
						printf("Unknow argument {%s}\n", param);
					}
					
					exit_code[pc++] = (unsigned char)atoi(param1);
				} else if(strcmp(inst, "MOV") == 0) {
					GetParam(as, param);
					GetParam(as, param1);
					
					int dest_idx = GetRegIndex(param);
					int src_idx = GetRegIndex(param1);
					if(dest_idx != -1 && src_idx != -1) {
						exit_code[pc++] = 0x40 + (dest_idx * 8) + src_idx;
					} else printf("Unknow argument {%s}\n", param);
				
				} else if(strcmp(inst, "HLT") == 0) {
					exit_code[pc++] = 0x76;
				} else {
					printf("Unknow instruction {%s}\n", inst);
				}
				
				index = 0;
			}
		} else {
			inst[index++] = c;
		}
	}
	
	fwrite(exit_code, sizeof(unsigned char), 128, bin);
	
	fclose(as);
	fclose(bin);
	return 0;
}
