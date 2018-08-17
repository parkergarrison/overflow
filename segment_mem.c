#include <stdio.h>
#include <stdlib.h>
extern int etext, edata, end;

static int d;

int main( ){
	printf("Address of etext: %08x\n", (int)&etext);
	printf("Address of edata: %08x\n", (int)&edata);
	printf("Address of end: %08x\n", (int)&end);

	char *dataAddr = "hello"; //in initialized data segment
	static int a=1; //in initialized data segment
	static int bssAddr; //in uninitialized data segment
	char s2[] = "hello"; //in the stack area.
	
	void* heapAddr = malloc(1000);
	int* mainPtr = &main;

	printf("Address in .text: %08x\n", *mainPtr);
	printf("Address in .data: %08x\n", dataAddr);
	printf("Address in .data: %08x\n", &a);
	printf("Address in .bss: %08x\n", &bssAddr);
	printf("Address in heap: %08x\n", heapAddr);
	printf("Address in stack: %08x\n", s2);
	return 0;

}


