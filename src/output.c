#include <stdio.h>
#include "psmem.h"

void printHeader(void) {
	printf(" Private  +   Shared  =  RAM used\tProgram\n\n");
}
void printTotal(double total) {
	printf("---------------------------------\n");
	printf("%24s"," ");
	calculateSize(total);
	printf("\n=================================\n");

}

void calculateSize(double rawSize) {
	const char* const sizes[] = {"KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB"};
	int maxOrder = sizeof(sizes) / sizeof(sizes[0]);
	int order = 0;
	while (rawSize >= 1000 && order <  maxOrder) {
		rawSize /= 1024.0;
		order++;
	}
	/* our memory is higher than YiB */
	if (order == maxOrder) {
		printf("%9s","MEMOUT");
	} else {
	printf("%5.1f %3s", rawSize, sizes[order]);
	}
}

void printSizes(struct program p) {
	calculateSize(p.private_mem);
	printf(" + ");
	calculateSize(p.shared_mem);
	printf(" = ");
	calculateSize(p.private_mem + p.shared_mem);
	printf("\t%.*s",NAME_SIZE - 1, p.name);
	if (p.count > 0) {
		printf(" (%d)", p.count + 1);
	}
	printf("\n");
}


