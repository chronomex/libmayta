/*
 * Copyright (c) 2015, Duncan Smith <duncan@xrtc.net>
 *
 * All rights reserved.  This file is licensed under version 3 of the
 * GNU GPL, or (at your option) any later version.
 */

#include "mayta.h"
#include "bloom.h"
#include <stdio.h>

int mayta_init(struct mayta** mayta, size_t bytes, int entries, double error)
{
	struct mayta* m;
	int bit;

	printf("Allocating a %d bit lossy array...\n", (int)bytes*8);

	m = calloc(sizeof(struct mayta) + (8 * bytes * sizeof(struct bloom*)), 1);
	m->bits = bytes * 8;

	for (bit = 0; bit < m->bits; bit++) {
		m->d[bit] = calloc(sizeof(struct bloom), 1);
		bloom_init(m->d[bit], entries, error);
	}
	*mayta = m;
	return 0;
}

int mayta_free(struct mayta* m)
{
	int bit;
	for (bit = 0; bit < m->bits; bit++) {
		bloom_free(m->d[bit]);  m->d[bit] = NULL;
	}
	return 0;
}

// Returns the number of bits which may be in error.
int mayta_store(struct mayta* m, size_t keysize, void* key, size_t bytes, void* data)
{
	int byte, bit;
	int err = 0;
	char* d = (char*) data;

	if (bytes * 8 > m->bits) return -1;
	for (byte = 0; byte < bytes; byte++)
		for (bit = 0; bit < 8; bit++) {
			int ret;
			struct bloom* column = m->d[byte*8 + bit];
			int (*fun)(struct bloom*, const void*, int);

			fun = (d[byte] & (1<<bit)) ? bloom_add : bloom_check;
			ret = (fun)(column, key, keysize);
//			fprintf(stdout, "%d/%d->%d/%d  ", byte, bit, d[byte] & (1<<bit) && 1, ret);
			if (ret == -1) return -1; // something is fucky
			err += ret; // +1 means the bit was already set (hrmmmm)
		}
	return err;
}

int mayta_get(struct mayta* m, size_t keysize, void* key, size_t bytes, void* data)
{
	int byte, bit;
	char* d = (char*) data;
	if (bytes * 8 > m->bits) return -1;
	for (byte = 0; byte < bytes; byte++)
		for (bit = 0; bit < 8; bit++) {
			struct bloom* column = m->d[byte*8 + bit];
			int ret = bloom_check(column, key, keysize);
			if (ret == 1) d[byte] |= (1 << bit);
			if (ret == -1) return -1; // something is fucky
		}
	return 0;
}
