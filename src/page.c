#include"page.h"
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include"util.h"


void moveCursorDown(PageCursor *cursor, int maxHeight) {
	cursor->x = 0;
	cursor->y = cursor->y + 1;
	if (cursor->y >= maxHeight) {
		cursor->y = 0;
		cursor->section = cursor->section + 1;
	}
}

Page *createPage(int sectionCount, int width, int height, int spacing) {
	Page* page = (Page*)malloc(sizeof(Page));
	// Allocate PageSection Stuff
	page->sectionCount = sectionCount;
	page->sectionWidth = width;
	page->sectionHeight = height;
	page->sections = (PageSection*)malloc(sizeof(PageSection) * sectionCount);
	for (int i = 0; i < sectionCount; i++) {
		page->sections[i].linesSizes = (int*)malloc(sizeof(int) * height);
		page->sections[i].lines = (char**)malloc(sizeof(char*) * height);
		for (int j = 0; j < height; j++) {
			page->sections[i].linesSizes[j] = 0;
			page->sections[i].lines[j] = (char*)malloc(sizeof(char) * width);
			memset(page->sections[i].lines[j], '\0', width); // We set all the lines ad empty lines
		}
	}
	// Allocate PageCursor stuff
	page->cursor = (PageCursor*)malloc(sizeof(PageCursor));
	page->cursor->x = 0;
	page->cursor->y = 0;
	page->cursor->section = 0;
	// Allocate Page stuff
	page->spacing = spacing;
	return page;
}

void freePage(Page* page) {
	// Free PageSection stuff
	for (int i = 0; i < page->sectionCount; i++) {
		for (int j = 0; j < page->sectionHeight; j++) {
			free(page->sections[i].lines[j]);
		}
		free(page->sections[i].lines);
		free(page->sections[i].linesSizes);
	}
	// Free PageCursor Stuff
	free(page->cursor);
	// Free Page Stuff
	free(page->sections);
	free(page);
}

int isFull(Page* page) {
	// If the current section of the cursor exceed the sectionCount of the page, the page is full.
	if (page->cursor->section >= page->sectionCount) {
		return 1;
	}
	return 0;
}

int insertWord(char* word, int wordLength, Page* page) {
	// Check the cursor validity
	if (isFull(page)) {
		// If the page is full, we send 0 to indicate that the word cannot be inserted into this page.
		return 0;
	}
	PageSection section = page->sections[page->cursor->section];
	int currentLineSize = u8strlen(section.lines[page->cursor->y]);
	if (currentLineSize + u8strlen(word) > page->sectionWidth) { // The word exceed the PageSection width. Need to go down.
		fillLine(page);
		return insertWord(word, wordLength, page); // Retry to insert the word in the new section.
	}
	int bufferSize = section.linesSizes[page->cursor->y];
	if (bufferSize < page->cursor->x + wordLength) { // Increment the lines[page->cursor->y] buffer if the word doesn't fit in.
		section.lines[page->cursor->y] = realloc(section.lines[page->cursor->y], page->cursor->x + wordLength);
		section.linesSizes[page->cursor->y] = page->cursor->x + wordLength;
		bufferSize = section.linesSizes[page->cursor->y];
	}
	for (int i = 0; i < wordLength; i++) {// Copy the word into the page
		section.lines[page->cursor->y][page->cursor->x + i] = word[i];
	}
	// Check the new line size
	page->cursor->x = page->cursor->x + wordLength;
	if (u8strlen(section.lines[page->cursor->y]) < page->sectionWidth) {
		if (bufferSize <= page->cursor->x) { // If the buffer for the lines[page->cursor->y] is too small, we grow it.
			section.lines[page->cursor->y] = realloc(section.lines[page->cursor->y], page->cursor->x + 1);
			section.linesSizes[page->cursor->y] = page->cursor->x + 1;
			bufferSize = section.linesSizes[page->cursor->y];
		}
		section.lines[page->cursor->y][page->cursor->x] = 32;
		page->cursor->x++;
	}
	if (u8strlen(section.lines[page->cursor->y]) >= page->sectionWidth) {
		fillLine(page);
	}
	return 1;
}

void fillLine(Page* page) {
	if (isFull(page)) {
		return;
	}
	PageSection section = page->sections[page->cursor->section];
	int lineSize = u8strlen(section.lines[page->cursor->y]);
	while(lineSize < page->sectionWidth) {
		section.lines[page->cursor->y][page->cursor->x] = 32;
		page->cursor->x++;
		lineSize++;
	}
	if (strlen(section.lines[page->cursor->y]) > section.linesSizes[page->cursor->y]) {
		// Increment the linesSize of the line filled.
		section.linesSizes[page->cursor->y] = strlen(section.lines[page->cursor->y]);
	}
	moveCursorDown(page->cursor, page->sectionHeight);
}

void indent(Page* page) {
	int width = page->sectionWidth;
	for (int i = 0; i < page->sectionCount; i++) {
		PageSection section = page->sections[i];
		for (int j = 0; j < page->sectionHeight; j++) {
			char* line = section.lines[j];
			int bufferSize = section.linesSizes[j];
			int spaceCount = 2; // Counts how many space should be between two words.
			int spaceIndex = bufferSize - 1; // We search spaces from the end of the line.
			if (spaceIndex == -1 || line[spaceIndex] != ' ') { // No space to distribute. Go to next line.
				continue;
			}
			int wordIndex = nextWordIndex(line, 0, width);
			char tmp[bufferSize];
			while (line[spaceIndex] == ' ') { // We will stop this cicle untile the spaces to the end are all distributed.
				stringCopy(tmp, 0, wordIndex + spaceCount - 1, line, 0, wordIndex + spaceCount - 1);
				tmp[wordIndex + spaceCount - 1] = ' ';
				stringCopy(tmp, wordIndex + spaceCount, bufferSize, line, wordIndex + spaceCount - 1, bufferSize - 1);
				stringCopy(line, 0, bufferSize, tmp, 0, bufferSize);
				int next = nextWordIndex(line, wordIndex + spaceCount, width);
				if (next == -1) { // Reached end of line. Restarting index.
					next = nextWordIndex(line, 0, width);
					if (next == wordIndex) { // Can't distribute space with only one word.
						break;
					}
					spaceCount++;
				}
				wordIndex = next;
			}
		}
	}
}

void printPage(Page* page) {
	printPageOn(page, stdout);
}

void printPageOn(Page* page, FILE* strm) {
	fillLine(page);
	indent(page);
	int width = page->sectionWidth;
	int height = page->sectionHeight;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j <= page->cursor->section; j++) {
			// Check if we reached the end
			if (j == page->sectionCount) {
				break;
			}
			if (i > page->cursor->y && page->cursor->section == j) {
				break;
			}
			if (j != 0) { // Print spaces
				for (int k = 0; k < page->spacing; k++) {
					fprintf(strm, " ");
				}
			}
			fprintf(strm, "%s", page->sections[j].lines[i]);
		}
		fprintf(strm, "\n");
	}
	fprintf(strm, "\n %%% \n\n");
}

char* serializePage(Page* page) {
	int sectionsSize =  ((4 * page->sectionHeight) + (page->sectionWidth * page->sectionHeight)) * page->sectionCount;
	// Calcolate Length of data.
	int totalLength = 32; // Page and Cursor General Data Size
	for (int i = 0; i < page->sectionCount; i++) { // Line Structs sizes
		for (int j = 0; j < page->sectionHeight; j++) {
			totalLength += 4 + page->sections[i].linesSizes[j];
		}
	}
	char* data = (char*)calloc(totalLength, sizeof(char));
	int offset = 0;
	writeInt(totalLength, data, &offset);
	writeInt(page->sectionCount, data, &offset);
	writeInt(page->sectionWidth, data, &offset);
	writeInt(page->sectionHeight, data, &offset);
	writeInt(page->spacing, data, &offset);
	writeInt(page->cursor->x, data, &offset);
	writeInt(page->cursor->y, data, &offset);
	writeInt(page->cursor->section, data, &offset);
	for (int i = 0; i < page->sectionCount; i++) {
		for (int j = 0; j < page->sectionHeight; j++) {
			writeInt(page->sections[i].linesSizes[j], data, &offset);
			for (int k = 0; k < page->sections[i].linesSizes[j]; k++) {
				data[offset] = page->sections[i].lines[j][k];
				offset++;
			}
		}
	}
	return data;
}

Page* deserializePage(char* data) {
	int offset = 4;
	// Read Page General data
	int sectionCount = getInt(data, &offset);
	int sectionWidth = getInt(data, &offset);
	int sectionHeight = getInt(data, &offset);
	int spacing = getInt(data, &offset);
	Page* page = createPage(sectionCount, sectionWidth, sectionHeight, spacing);
	// Read Cursor Data
	page->cursor->x = getInt(data, &offset);
	page->cursor->y = getInt(data, &offset);
	page->cursor->section = getInt(data, &offset);
	// Read Line Structs
	for (int i = 0; i < sectionCount; i++) {
		for (int j = 0; j < sectionHeight; j++) {
			page->sections[i].linesSizes[j] = getInt(data, &offset);
			for (int k = 0; k < page->sections[i].linesSizes[j]; k++) {
				page->sections[i].lines[j][k] = data[offset];
				offset++;
			}
		}
	}
	return page;
}
