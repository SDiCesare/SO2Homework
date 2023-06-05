#ifndef UTIL_H
#define UTIL_H
#include<stdio.h>

/**
 * This header contains utility functions for the program.
 * */

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

/**
 * Finds the first index of the next word in a line.
 * The next word is defined as the word after a space character (32).
 * 
 * @param line: The line to scan.
 * @param start: The index from which the line is scanned.
 * @param end: The last index of the line. 
 * */
int nextWordIndex(char* line, int start, int end);

/**
 * A different version of the standard strcpy.
 * It copies source[sourceStar:sourceEnd] to dest[destStart:destEnd]
 * 
 * @param dest: Pointer to the destination string.
 * @param destStart: destination index from which the copy starts.
 * @param destEnd: destination index from which the copy ends.
 * @param source: Pointer to the source string.
 * @param sourceStart: source index from which the copy starts.
 * @param sourceEnd: source index from which the copy ends.
 * */
void stringCopy(char* dest, int destStart, int destEnd, char* source, int sourceStart, int sourceEnd);

/**
 * Calculate the characters contained on a UTF-8 string.
 * 
 * @param str: The UTF-8 String.
 * 
 * @return the number of the actual characters in the string
 * */
int u8strlen(const char* str);

/**
 * Prints the help output of this program when runned with the option -h or --help.
 * */
void printHelp();

/**
 * Writes an integer in Little Indian to a char buffer.
 * 
 * @param n: The integer to write
 * @param data: The char buffer
 * @param offset: The offset pointer at which n is written. It will be moved by 4 after the operation.  
 * */
void writeInt(int n, char* data, int* offset);

/**
 * Retrieve an integer in Little Indian from a char buffer.
 * 
 * @param data: The char buffer
 * @param offset: The offset pointer at which n is retrieved. It will be moved by 4 after the operation.  
 * 
 * @return The integer read.
 * */
int getInt(char* data, int* offset);

#endif
