#ifndef PAGE_H
#define PAGE_H
#include<stdio.h>

/**
 * The PageCurstor data structure.
 * It rapresent a cursor moving through a Page while writing on it.
 * @param x: The x position on the PageSection pointed by a Cursor.
 * @param y: The y position on the PageSection pointed by a Cursor.
 * @param section: The index of the PageSection pointed by a Cursor.
 * */
typedef struct {
	int x;
	int y;
	int section;
} PageCursor;

/**
 * Moves the cursor y down to one, and the cursor x to 0 (beginning of line).
 * If the new y is greather than maxHeight, than the y is set to 0 and the section is incremented by one.
 * @param cursor: A pointer to a PageCursor that is moved down.
 * @param maxHeight: The maximum value that the cursor y can assume. 
 * */
void moveCursorDown(PageCursor* cursor, int maxHeight);

/**
 * The PageSection data structure.
 * It represent a single column within a Page.
 * @param linesSizes: Store the size of each line. Used for UTF-8 encoding.
 * @param lines: The content of the PageSection. Rapresented as a matrix widthXheight of characters.
 * @see Page
 * */
typedef struct {
	int* linesSizes;
	char** lines;
} PageSection;

/**
 * The Page data structure.
 * @param sectionCount: The number of PageSection componing a Page.
 * @param sectionWidth: The number of characters per row of a PageSection componing a Page.
 * @param sectionHeight: The number of characters per column of a PageSection componing a Page.
 * @param sections: The list of the PageSections componing a Page.
 * @param spacing: The number of space characters between one PageSection and another.
 * @param cursor: The cursor used for moving through the PageSections while writing a Page.
 * */
typedef struct {
	int sectionCount;
	int sectionWidth;
	int sectionHeight;
	PageSection* sections;
	int spacing;
	PageCursor* cursor;
} Page;

/**
 * Allocates a new Page with the specified arguments.
 * @see Page
 * @param sectionCount: The Page sectionCount.
 * @param width: The PageSection width.
 * @param height: The PageSection height.
 * @param spacing: The Page spacing.
 * @return A Page pointer pointing to the newly allocated Page.
 * */
Page *createPage(int sectionCount, int width, int height, int spacing);

/**
 * Free the previously allocated memory of a Page.
 * @see Page
 * @param page: A Page pointer that will be free.
 * */
void freePage(Page* page);

/**
 * Checks if the page is Full or has free space to be written.
 * @param page: A Page pointer pointing to the Page to check.
 * @return 1 if the page is full, 0 otherwise.
 * */
int isFull(Page* page);

/**
 * Insert a word into a Page, following a column schema.
 * @param word: The word to be inserted.
 * @param wordLength: The length of the word. This length is the byte-length of the word, not the number of character displayed on screen.
 * @param page: A Page pointer pointing to the page in which the word is written.
 * @return 1 If the word is inserted on the Page, 0 otherwise.
 * */
int insertWord(char* word, int wordLength, Page* page);

/**
 * Fills the line in which the PageCursor of the page is in with white spaces.
 * @param page: A Page pointer pointing the page to be filled.
 * */
void fillLine(Page* page);

/**
 * Indents the page content.
 * @param page: A Page pointer pointing the page to be indented.
 * */
void indent(Page* page);

/**
 * Prints on stdout a page.
 * @param page: A Page pointer pointing the page to be printed.
 * */
void printPage(Page* page);

/**
 * Prints a page into a stream.
 * @param page: A Page pointer pointing the page to be printed.
 * @param strm: The stream in which the page is printed.
 * */
void printPageOn(Page* page, FILE* strm);

/**
 * 
 * */
char* serializePage(Page* page);

/**
 * 
 * */
Page* deserializePage(char* data);

#endif
