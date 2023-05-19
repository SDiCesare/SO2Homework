#ifndef MULTIPROCESS_H
#define MULTIPROCESS_H
#include"util.h"

/**
 * Runs a version of this program with 3 concurrent process:
 * The first one reads word per word and sends them to the second process.
 * The second one inserts the words passed by the first one in a Page. When the page is full, it calls the third process and reset the page.
 * The third one writes the page in the output file until the second one send a control bit for ending.
 * The main process stops when the third one stops.
 * */
void runMultiprocessingExecution(Arguments arguments);

void runReadingProcess(Arguments arguments, int wordPipe[]);

void runComputingProcess(Arguments arguments, int wordPipe[], int pagePipe[]);

void runWritingProcess(Arguments arguments, int pagePipe[]);

#endif
