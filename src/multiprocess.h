#ifndef MULTIPROCESS_H
#define MULTIPROCESS_H
#include"util.h"

/**
 * Runs a version of this program with 3 concurrent process:
 * The first one reads word per word and sends them to the second process.
 * The second one inserts the words passed by the first one in a Page. When the page is full, it calls the third process and reset the page.
 * The third one writes the page in the output file until the second one send a control bit for ending.
 * The main process stops when the third one stops.
 * 
 * @param arguments: The terminal arguments.
 * */
void runMultiprocessingExecution(Arguments arguments);

/**
 * This method reads word by word the input file, and send them to the computingProcess through the wordPipe.
 * 
 * @param arguments: The terminal arguments.
 * @param wordPipe: The pipe that link the readingProcess with the computingProcess.
 * */
void runReadingProcess(Arguments arguments, int wordPipe[]);

/**
 * This method receive a word from the readingProcess and insert it in a Page.
 * If the current Page is full, it serializes and sends it to the writingProcess through the pagePipe.
 * 
 * @param arugments: The terminal arguments.
 * @param wordPipe: The pipe that link the readingProcess with the computingProcess.
 * @param pagePipe: The pipe that link the computingProcess with the writingProcess.
 * */
void runComputingProcess(Arguments arguments, int wordPipe[], int pagePipe[]);

/**
 * This method receive a serialized Page from the computingProcess and print it to the output file.
 * 
 * @param arugments: The terminal arguments.
 * @param pagePipe: The pipe that link the computingProcess with the writingProcess.
 * */
void runWritingProcess(Arguments arguments, int pagePipe[]);

#endif
