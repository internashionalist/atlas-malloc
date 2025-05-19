#ifndef MALLOC_H
#define MALLOC_H

#include <unistd.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>

/**
 * struct block_header - header for a memory block in the allocator
 * @size: payload size in bytes.
 * @free: 1 if the block is available, 0 if in use.
 * @next: pointer to the next block in the singly-linked list.
 *
 * This structure is used to manage memory blocks in the allocator.
 */
typedef struct block_header
{
	size_t size;			   /* payload size (bytes) */
	int free;				   /* 1 = available, 0 = in use */
	struct block_header *next; /* singly‑linked list */
} block_header_t;

void *naive_malloc(size_t);
void *_malloc(size_t);
void _free(void *ptr);

#endif
