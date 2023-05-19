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
			memset(page->sections[i].lines[j], '\0', width);
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
	for (int i = 0; i < page->sectionCount; i++) {
		for (int j = 0; j < page->sectionHeight; j++) {
			//printf("%d - ", page->sections[i].lines[j]);
			free(page->sections[i].lines[j]);
		}
		//printf("\n%d", page->sections[i].lines);
		free(page->sections[i].lines);
		//printf("%d", page->sections[i].linesSizes);
		free(page->sections[i].linesSizes);
	}
	//printf("%d %d %d", page->sections, page->cursor, page);
	free(page->sections);
	free(page->cursor);
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
	for (int i = 0; i < wordLength; i++) {
		section.lines[page->cursor->y][page->cursor->x + i] = word[i];
	}
	page->cursor->x = page->cursor->x + wordLength;
	if (u8strlen(section.lines[page->cursor->y]) < page->sectionWidth) {
		if (bufferSize <= page->cursor->x) {
			section.lines[page->cursor->y] = realloc(section.lines[page->cursor->y], page->cursor->x + 1);
			section.linesSizes[page->cursor->y] = page->cursor->x + 1;
			bufferSize = section.linesSizes[page->cursor->y];
		}
		section.lines[page->cursor->y][page->cursor->x] = 32;
		page->cursor->x++;
	}
	if (page->cursor->x >= page->sectionWidth) {
		page->cursor->x = 0;
		page->cursor->y++;
		if (page->cursor->y >= page->sectionHeight) {
			page->cursor->y = 0;
			page->cursor->section++;
		}
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
			// printf("Buffer: %d - %d - %d\t'%s'\n", bufferSize, strlen(line), u8strlen(line), line);
			if (spaceIndex == -1 || line[spaceIndex] != ' ') { // No space to distribute. Go to next line.
				continue;
			}
			int wordIndex = nextWordIndex(line, 0, width);
			char tmp[bufferSize]; //= (char*)malloc(sizeof(char) * bufferSize);
			while (line[spaceIndex] == ' ') { // We will stop this cicle untile the spaces to the end are all distributed.
				stringCopy(tmp, 0, wordIndex + spaceCount - 1, line, 0, wordIndex + spaceCount - 1);
				tmp[wordIndex + spaceCount - 1] = ' ';
				stringCopy(tmp, wordIndex + spaceCount, bufferSize, line, wordIndex + spaceCount - 1, bufferSize - 1);
				// printf("'%s' -> '%s'\n", line, tmp);
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
				// printf("%d == 32 %d? for %d\n", line[spaceIndex], line[spaceIndex] == ' ', j);
			}
			// free(tmp);
			// printf("%d -> '%s'\n", bufferSize, line);
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
			if (j == page->sectionCount) {
				break;
			}
			if (i > page->cursor->y && page->cursor->section == j) {
				break;
			}
			if (j != 0) { // Print spaces
				for (int k = 0; k < page->spacing; k++) {
					fputs(" ", strm);
				}
			}
			/*if (strlen(page->sections[j].lines[i]) == 0) { // Anticipated End. Happends only on the last page.
				return;
			}*/
			fputs(page->sections[j].lines[i], strm);
		}
		fputs("\n", strm);
	}
	fputs("\n %%% \n\n", strm);
}

char* serializePage(Page* page) {
	int sectionsSize =  ((4 * page->sectionHeight) + (page->sectionWidth * page->sectionHeight)) * page->sectionCount;
	int totalLength = 16 + 12 + sectionsSize + 4;
	// printf("Page Size: %d\n", totalLength);
	char* data = (char*)malloc(totalLength);
	int offset = 0;
	// Writing Structure Size and General Page Data
	writeInt(totalLength, data, &offset);
	writeInt(page->sectionCount, data, &offset);
	writeInt(page->sectionWidth, data, &offset);
	writeInt(page->sectionHeight, data, &offset);
	writeInt(page->spacing, data, &offset);
	// Writing Page Cursor Data
	writeInt(page->cursor->x, data, &offset);
	writeInt(page->cursor->y, data, &offset);
	writeInt(page->cursor->section, data, &offset);
	// Writing PageSections Data
	for (int i = 0; i < page->sectionCount; i++) {
		PageSection section = page->sections[i];
		for (int j = 0; j < page->sectionHeight; j++) { // Write line Sizes of PageSection
			writeInt(section.linesSizes[j], data, &offset);
		}
		for (int j = 0; j < page->sectionHeight; j++) { // Write content of PageSection
			for (int k = 0; k < page->sectionWidth; k++) {
				data[offset] = section.lines[j][k];
				offset++;
			}
		}
	}
	return data;
}

Page* deserializePage(char* data) {
	Page* page = (Page*)malloc(sizeof(Page));
	int offset = 4;
	// Read Page General data
	page->sectionCount = getInt(data, &offset);
	page->sectionWidth = getInt(data, &offset);
	page->sectionHeight = getInt(data, &offset);
	page->spacing = getInt(data, &offset);
	// Read PageCursor data
	page->cursor = (PageCursor*)malloc(sizeof(PageCursor));
	page->cursor->x = getInt(data, &offset);
	page->cursor->y = getInt(data, &offset);
	page->cursor->section = getInt(data, &offset);
	// Read PageSections data
	page->sections = (PageSection*)malloc(sizeof(PageSection) * page->sectionCount);
	for (int i = 0; i < page->sectionCount; i++) {
		page->sections[i].linesSizes = (int*)malloc(sizeof(int) * page->sectionHeight);
		for (int j = 0; j < page->sectionHeight; j++) {
			page->sections[i].linesSizes[j] = getInt(data, &offset);
		}
		page->sections[i].lines = (char**)malloc(sizeof(char*) * page->sectionHeight);
		for (int j = 0; j < page->sectionHeight; j++) {
			page->sections[i].lines[j] = (char*)malloc(sizeof(char) * page->sectionWidth);
			for (int k = 0; k < page->sectionWidth; k++) {
				page->sections[i].lines[j][k] = data[offset];
				offset++;
			}
		}
	}
	return page;
}
