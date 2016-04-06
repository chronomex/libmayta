
#define _GNU_SOURCE
#include "mayta.h"
#include <stdio.h>
//#include <getline.h>

int main(int argc, char** argv)
{
	struct mayta* mayta;
	int nr = 0;
	char* line;
	size_t len;

	mayta_init(&mayta, 255, 100, 0.001);

	do {
		char data[256];
		long int ber;
		line = calloc(len = 255, 1);
		memset(data, 0, 256);
		if (getline(&line, &len, stdin) == -1) break;

		ber = mayta_store(mayta, sizeof(long int), (void*) &nr, len, line);
		printf("berr = %d\t", ber);

		mayta_get(mayta, sizeof(long int), (void*) &nr, len, &data);
		data[255] = 0; // null-terminate it so we don't overrun on print
		printf("%s", &data);
		nr = random();
	} while (1);
}
