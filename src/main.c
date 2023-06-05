#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include"page.h"
#include"util.h"
#include"multiprocess.h"

void badUsageError() {
	printf("Bad Usage\nTry running with -h or --help for more information.\n");
	exit(0);
}


/**
 * Read the terminal arguments for running this program.
 * Options:
 * -c <int>: Specify the number of columns for the output file
 * -s <int>: Specify the number of blank spaces between 2 columns.
 * -o <file>: Specify the output file in which the formatted text is printed.
 * -mp --multiprocess: Will run the program as multiprocess
 * -h --help: Print information about the usage.
 * Parameters:
 * width <int>: The number of character per row.
 * height <int>: The number of row per column.
 * file <file>: The file from which the text is read.
 * 
 * @param argc: The argument count from terminal
 * @param argv: The arguments from terminal
 * */
Arguments handleTerminalArguments(int argc, char* argv[]) {
	Arguments arguments;// = (Arguments*)malloc(sizeof(Arguments));
	if (argc == 2) {
		if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
			printHelp();
		} else {
			badUsageError();
		}
	}
	if (argc < 4) {
		badUsageError();
	}
	arguments.sections = 1;
	arguments.spacing = 4;
	arguments.outputFile = "out.txt";
	arguments.width = atoi(argv[argc - 3]);
	arguments.height = atoi(argv[argc - 2]);
	arguments.inputFile = argv[argc - 1];
	arguments.multiprocess = 0;
	int maxOptionIndex = argc - 3;
	for (int i = 1; i < maxOptionIndex; i++) {
		char* arg = argv[i];
		if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
			printHelp();
		} else if (strcmp(arg, "-s") == 0) {
			if (i < maxOptionIndex - 1) {
				arguments.spacing = atoi(argv[i + 1]);
				i++;
			} else {
				badUsageError();
			}
		} else if (strcmp(arg, "-c") == 0) {
			if (i < maxOptionIndex - 1) {
				arguments.sections = atoi(argv[i + 1]);
				i++;
			} else {
				badUsageError();
			}
		} else if (strcmp(arg, "-o") == 0) {
			if (i < maxOptionIndex - 1) {
				arguments.outputFile = argv[i + 1];
				i++;
			} else {
				badUsageError();
			}
		} else if (strcmp(arg, "--multiprocess") == 0 || strcmp(arg, "-mp") == 0) {
			arguments.multiprocess = 1;
		}
	}
	return arguments;
}

int main(int argc, char* argv[]) {
	// Sections, Width, Height, Spacing
	Arguments arguments = handleTerminalArguments(argc, argv);
	if (arguments.multiprocess) {
		runMultiprocessingExecution(arguments);
		return 0;
	}
	// printf("%d, %d, %d, %d, %s, %s\n", arguments.sections, arguments.width, arguments.height, arguments.spacing, arguments.inputFile, arguments.outputFile);
	Page* page = createPage(arguments.sections, arguments.width, arguments.height, arguments.spacing);
	// Open files
	FILE *out;
	if (arguments.outputFile == "stdout") {
		out = stdout;
	} else {
		out = fopen(arguments.outputFile, "w+");
	}
	if (out == 0) {
		printf("Can't open output file '%s'\n", arguments.outputFile);
		return -1;
	}
	FILE *fptr;
	fptr = fopen(arguments.inputFile, "r");
	if (fptr == 0) {
		printf("Can't open input file %s!\n", arguments.inputFile);
		return -1;
	}
	// Create word info
	char* word = calloc(arguments.width + 1, sizeof(char));
	int wordSize = arguments.width + 1;
	int wordIndex = 0;
	int errorFlag = 0;
	printf("Computing File '%s'\n", arguments.inputFile);
	while (!feof(fptr)) {
		char c = fgetc(fptr);
		if (c == 32 || c == 10) { // We reached a word separator.
			if (wordIndex == 0) { // We have no word yet.
				continue;
			}
			int inserted = insertWord(word, strlen(word), page);
			if (!inserted) { // If the word doesn't fit in the page, the page is full
				printPageOn(page, out);
				freePage(page);
				page = createPage(arguments.sections, arguments.width, arguments.height, arguments.spacing);
				inserted = insertWord(word, strlen(word), page);
				if (!inserted) {
					printf("There was an error while creating a new page.\nAborting.\n");
					errorFlag = 1;
					break;
				}
			}
			free(word);
			word = calloc(arguments.width + 1, sizeof(char));
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
			printf("Encountered a word with length grater than the specified width!\nAborting.\n", word);
			errorFlag = 1;
			break;
		}
	}
	int closed = fclose(fptr);
	if (closed != 0) {
		printf("Error Closing Input File '%s'\n", arguments.inputFile);
		return -1;
	}
	if (errorFlag) {
		return -1;
	} else {
		printPageOn(page, out);
		if (strcmp(arguments.outputFile, "stdout") != 0) {
			printf("File '%s' created successfully!\n", arguments.outputFile);
		}
		return 0;
	}
}
