#include"util.h"
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>

int nextWordIndex(char* line, int start, int end) {
	for (int i = start; i < end; i++) {
		if (line[i] == ' ') {
			return i;
		}
	}
	return -1;
}

void stringCopy(char* dest, int destStart, int destEnd, char* source, int sourceStart, int sourceEnd) {
	for (int i = destStart, j = sourceStart; i < destEnd && j < sourceEnd; i++, j++) {
		dest[i] = source[j];
	}
}

int u8strlen(const char* str) {
	int len = 0;
	while(*str) {
		if ((*str & 0xC0) != 0x80) { // If the character value is not a UTF-8 identifier, than we count the character.
			len = len + 1;
		}
		str++;
	}
	return len;
}

long int getFileSize(FILE* file) {
	fseek(file, 0L, SEEK_END);
	long int size = ftell(file);
	rewind(file);
	return size;
}

void printHelp() {
	printf("Usage: ./main [OPTIONS] width height file\n");
	printf("Options:\n");
	printf("\t-s <int>\tSet the number of spaces between one column and another. Default=4.\n");
	printf("\t-c <int>\tSet the number of columns per page. Default=1.\n");
	printf("\t-o <file>\tPlace the output into <file>. Default='out.txt'.\n");
	printf("\t-mp --multiprocess\tRun programm as multiprocess.\n");
	printf("\t-h --help\tPrint information about usage.\n\n");
	printf("Parameters:\n");
	printf("\twidth <int>\tThe number of characters per row of a column.\n");
	printf("\theight <int>\tThe number of rows of a column.\n");
	printf("\tfile <file>\tThe input file that contains the UTF-8 text to format.\n");
	exit(0);
}


void writeInt(int n, char* data, int* offset) {
	if (offset == 0) {
		data[0] = (n >> 24) & 0xFF;
		data[0 + 1] = (n >> 16) & 0xFF;
		data[0 + 2] = (n >> 8) & 0xFF;
		data[0 + 3] = n & 0xFF;
	} else {
		data[*offset] = (n >> 24) & 0xFF;
		data[*offset + 1] = (n >> 16) & 0xFF;
		data[*offset + 2] = (n >> 8) & 0xFF;
		data[*offset + 3] = n & 0xFF;
		*offset = *offset + 4;
	}
}

int getInt(char* data, int* offset) {
	if (offset == 0) {
		int n = ((data[0] << 24) & 0xFF000000);
		n = n | ((data[1] << 16) & 0x00FF0000);
		n = n | ((data[2] << 8) & 0x0000FF00);
		n = n | (data[3] & 0xFF);
		return n;
	} else {
		int n = ((data[*offset] << 24) & 0xFF000000);
		n = n | ((data[*offset + 1] << 16) & 0x00FF0000);
		n = n | ((data[*offset + 2] << 8) & 0x0000FF00);
		n = n | (data[*offset + 3] & 0xFF);
		*offset = *offset + 4;
		return n;
	}
}


int getIntFromPipe(int pipe[]) {
	char data[4];
	read(pipe[0], data, 4);
	return getInt(data, 0);
}
