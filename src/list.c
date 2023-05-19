#include"list.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct List *createList(int typeSize, int size) {
	struct List* list = (struct List*)malloc(sizeof(struct List));
	list->typeSize = typeSize;
	list->size = size;
	list->latestValue = 0;
	list->data = malloc(list->size * list->typeSize);
	return list;
}

void insertValue(struct List* list, void* value) {
	if (list->latestValue == list->size) {
		printf("Can't grow list for now!\n");
	} else {
		char *ptr = (char*)list->data;
		ptr = ptr + (list->typeSize * list->latestValue);
		memcpy(ptr, value, list->typeSize);
		list->latestValue = list->latestValue + 1;
	}
}

void *getValue(struct List* list, int index) {
	char *ptr = (char*)list->data;
	ptr = ptr + (list->typeSize * index);
	return ptr;
}

