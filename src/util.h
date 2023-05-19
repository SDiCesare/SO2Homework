#ifndef UTIL_H
#define UTIL_H
#include<stdio.h>

// Global Parameters Structur
typedef struct {
	int sections;
	int width;
	int height;
	int spacing;
	char* inputFile;
	char* outputFile;
	int multiprocess;
} Arguments;


int nextWordIndex(char* line, int start, int end);

void stringCopy(char* dest, int destStart, int destEnd, char* source, int sourceStart, int sourceEnd);

int u8strlen(const char* str);

long int getFileSize(FILE* file);

void printHelp();

void writeInt(int n, char* data, int* offset);

int getInt(char* data, int* offset);

int getIntFromPipe(int pipe[]);

#endif
