#pragma once
#include <stdlib.h>

struct mayta {
  size_t bits;
  struct bloom* d[];
};

int mayta_init(struct mayta** m, size_t bytes, int entries, double error);
int mayta_free(struct mayta* m);

int mayta_store(struct mayta* m, size_t keysize, void* key, size_t bytes, void* data);
int mayta_get(struct mayta* m, size_t keysize, void* key, size_t bytes, void* data);


