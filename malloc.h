#ifndef MALLOC_H
#define MALLOC_H

#include <unistd.h>
#include <stdalign.h>

void *naive_malloc(size_t);
void *_malloc(size_t);
void _free(void *ptr);

#endif
