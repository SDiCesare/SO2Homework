#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<signal.h>
#include"multiprocess.h"
#include"util.h"
#include"page.h"

/**
 * Checks if the pid is a valid process ID.
 * If its not valid, than the program is closed instantly.
 * 
 * @param pid: The Process ID
 * */
void checkPid(pid_t pid) {
	if (pid < 0) {
		printf("Error While Generating Process.\n");
		exit(-1);
	}
}

void runMultiprocessingExecution(Arguments arguments) {
	// Open pipes for process
	// Side 0 is for reading. Side 1 is for writing.
	int wordPipe[2];
	int pagePipe[2];
	if (pipe(wordPipe) < 0) {
		printf("Error Sharing Word Memory!\n");
		exit(-1);
	}
	if (pipe(pagePipe) < 0) {
		printf("Error Sharing Page Memory!\n");
		exit(-1);
	}
	pid_t pid = fork();
	checkPid(pid);
	if (pid == 0) { // Firs fork runs the reading process
		runReadingProcess(arguments, wordPipe);
	} else {
		pid = fork();
		checkPid(pid);
		if (pid == 0) { // Second fork runs the computing process
			runComputingProcess(arguments, wordPipe, pagePipe);
		} else {
			pid = fork();
			checkPid(pid);
			if (pid == 0) { // Third fork runs the writing process
				runWritingProcess(arguments, pagePipe);
			} else { // The father waits the three processes to close. If one ends with an error, the father kills all the other processes. 
				pid_t wpid;
				int status = 0;
				while ((wpid = wait(&status)) > 0) { // We are waiting for all the procss to finish
					printf("[%d]: terminated with status %d\n", wpid, status);
					if (status != 0) {
						printf("An error occured. Aborting Processing!\n");
						kill(0, SIGTERM);
					}
				}
				printf("File %s Created Successfully!\n", arguments.outputFile);
			}
		}
	}
}

void runReadingProcess(Arguments arguments, int wordPipe[]) {
	close(wordPipe[0]);// We close the reading side for the writing process.
	FILE* inputFile;
	inputFile = fopen(arguments.inputFile, "r");
	if (inputFile == 0) {
		 // We end the comunication by sending a -1
		printf("Can't open file %s!\n");
		int value = -1;
		write(wordPipe[1], &value, sizeof(int));
		close(wordPipe[1]);
		exit(-1);
	}
	char* word = calloc(arguments.width  + 1, sizeof(char));
	int wordSize = arguments.width + 1;
	int wordIndex = 0;
	int errorFlag = 0;
	while (!feof(inputFile)) {
		char c = fgetc(inputFile);
		if (c == 32 || c == 10) { // We reached a word separator.
			if (wordIndex == 0) { // No word found yet.
				continue;
			}
			int size = strlen(word);
			write(wordPipe[1], &size, sizeof(int)); // We send the length of the string.
			write(wordPipe[1], word, strlen(word)); // We send the word to the computingProcess
			free(word);
			word = calloc(arguments.width + 1, sizeof(char));
			if (!word) {
				// We end the comunication by sending a -1
				printf("Can't reallocate memory, Aborting!\n");
				int value = -1;
				write(wordPipe[1], &value, sizeof(int));
				close(wordPipe[1]);
				exit(-1);
			}
			wordSize = arguments.width + 1;
			wordIndex = 0;
		} else if ((c & 0xC0) == 0x80) { // UTF-8 Identifier first byte
			wordSize++;
			word = realloc(word, wordSize);
			word[wordIndex] = c;
			wordIndex++;
		} else { // Valid character found, append to the word
			word[wordIndex] = c;
			wordIndex++;
		}
		if (u8strlen(word) > arguments.width) { // Word too big, abort!
			// We end the comunication by sending a -1
			printf("Encountered a word with length grater than the specified width (%d). Aborting.\n", u8strlen(word));
			int value = -1;
			write(wordPipe[1], &value, sizeof(int));
			close(wordPipe[1]);
			exit(-1);
		}
	}
	printf("File readed. Reading Process Closed!\n");
	// We send a 0 to indicate that the pipe is closed.
	int value = 0;
	write(wordPipe[1], &value, sizeof(int));
	close(wordPipe[1]);
	fclose(inputFile);
    exit(0);
}

void runComputingProcess(Arguments arguments, int wordPipe[], int pagePipe[]) {
	close(wordPipe[1]);// We close the writing side for the reading process.
	close(pagePipe[0]);// We close the reading side for the writing process.
	Page* page = createPage(arguments.sections, arguments.width, arguments.height, arguments.spacing);
	int signal, byteRead;
	char* word;
	while (1) {
		byteRead = read(wordPipe[0], &signal, sizeof(int));
		if (byteRead == 0) {
			continue; // No int signal received
		}
		if (signal < 0) { // Error From Reading Process. Aborting!
			printf("Error On Reading Process, Aborting Computing!\n");
			close(wordPipe[0]);
			write(pagePipe[1], &signal, sizeof(int));
			close(pagePipe[1]);
			exit(-1);
		} else if (signal == 0) { // File complitely readed. Ending.
			break;
		}
		// In the signal is > 0, than is the length of the word received
		word = (char*)calloc(signal, sizeof(char));
		byteRead = read(wordPipe[0], word, signal);
		if (byteRead < signal) { // Error while receiving the word, Aborting!
			printf("Error while reading pipe for word. Sended %d bytes, Received %d!\n", signal, byteRead);
			signal = -1;
			close(wordPipe[0]);
			write(pagePipe[1], &signal, sizeof(int));
			close(pagePipe[1]);
			exit(-1);
		}
		int inserted = insertWord(word, strlen(word), page);
		if (!inserted) {
			// Serializing and Sending page to Writing Process.
			char* data = serializePage(page);
			int size = getInt(data, 0);
			write(pagePipe[1], &size, sizeof(int));
			write(pagePipe[1], data, size);
			freePage(page);
			page = createPage(arguments.sections, arguments.width, arguments.height, arguments.spacing);
			inserted = insertWord(word, strlen(word), page);
			if (!inserted) { // Should not happend unless the page was allocated uncorrectly
				printf("There was an error while creating a new page.\nAborting!\n");
				signal = -1;
				close(wordPipe[0]);
				write(pagePipe[1], &signal, sizeof(int));
				close(pagePipe[1]);
				exit(-1);
			}
		}
		free(word);
	}
	printf("No more word to receive. Computing Process Closed!\n");
	close(wordPipe[0]);
	// Sending last Page
	char* data = serializePage(page);
	int size = getInt(data, 0);
	write(pagePipe[1], &size, sizeof(int));
	write(pagePipe[1], data, size);
	// We send a 0 to indicate that the pipe is closed.
	signal = 0;
	write(pagePipe[1], &signal, sizeof(int));
	close(pagePipe[1]);
	exit(0);
}

void runWritingProcess(Arguments arguments, int pagePipe[]) {
	FILE *outputFile;
	if (arguments.outputFile == "stdout") {
		outputFile = stdout;
	} else {
		outputFile = fopen(arguments.outputFile, "w+");
	}
	if (outputFile == 0) { // Can't work on output file. Aborting!
		printf("Can't open output file '%s'\n", arguments.outputFile);
		close(pagePipe[1]);
		exit(-1);
	}
	close(pagePipe[1]); // We close the writing side for the reading process.
	int signal, byteRead;
	char* buffer;
	Page* page;
	while (1) {
		byteRead = read(pagePipe[0], &signal, sizeof(int));
		if (byteRead == 0) {
			continue; // No int signal received
		}
		if (signal < 0) { // Error From Computing Process. Aborting!
			printf("Error On Computing Process, Aborting Writing!\n");
			close(pagePipe[1]);
			exit(-1);
		} else if (signal == 0) { // No more Page to write. Ending!
			break;
		}
		// If the signal is > 0, than is the length of the serialized data of the Page sended.
		buffer = (char*)calloc(signal, sizeof(char));
		byteRead = read(pagePipe[0], buffer, signal);
		if (byteRead < signal) {
			printf("Error while reading pipe for Page. Sended %d bytes, received %d!\n", signal, byteRead);
			close(pagePipe[0]);
			exit(-1);
		}
		page = deserializePage(buffer);
		printPageOn(page, outputFile);
		freePage(page);
		free(buffer);
	}
	printf("Writing Process Ended.\n");
	close(pagePipe[0]);
	fclose(outputFile);
	exit(0);
}
