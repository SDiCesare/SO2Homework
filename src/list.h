#ifndef LIST_H
#define LIST_H

struct List {
	int typeSize;
	int size;
	int latestValue;
	void *data;
};

struct List *createList(int typeSize, int size);
void insertValue(struct List* list, void* value);
void *getValue(struct List* list, int index);

#endif
