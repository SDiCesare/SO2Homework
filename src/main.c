#include<stdio.h>
#include<stdlib.h>
#include"test.h"

int main(int argc, char* argv[]) {
	printf("%s", "Hello World!\n");
	int a = foo();
	printf("%d\n", a);
	return 0;
}