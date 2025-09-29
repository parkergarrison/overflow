#include <stdio.h>
#include <stdlib.h>
extern int etext, edata, end;

static int d;

int add(int a, int b){
	return a + b;
}

int main( ){
	printf("Address of etext: %0x\n", (int)&etext);
	printf("Address of edata: %0x\n", (int)&edata);
	printf("Address of end: %0x\n", (int)&end);

	char *dataAddr = "hello"; //in initialized data segment
	static int a=1; //in initialized data segment
	static int bssAddr; //in uninitialized data segment
	char s2[] = "hello"; //in the stack area.
	
	void* heapAddr = malloc(1000);
	size_t* mainPtr = (size_t*)&add;

	printf("Address in .text: %0x\n", mainPtr);
	printf("Address in .data: %0x\n", dataAddr);
	printf("Address in .data: %0x\n", &a);
	printf("Address in .bss: %0x\n", &bssAddr);
	printf("Address in heap: %0x\n", heapAddr);
	printf("Address in stack: %0x\n", s2);
	return 0;

}


