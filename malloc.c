#include <stdalign.h> /* alignof(max_align_t) */
#include <stddef.h>	  /* size_t */
#include <stdint.h>
#include <unistd.h> /* sbrk */
#include "malloc.h"

/* head of the block list */
static block_header_t *g_head;

/**
 * extend_heap - extend the heap by a given size
 * @size: size to extend the heap by
 *
 * This function extends the heap by a given size using sbrk.
 *
 * Return: pointer to the new block header
 */
static block_header_t *extend_heap(size_t size)
{
	size_t hdr = sizeof(block_header_t); /* size of the header */
	void *raw = sbrk(hdr + size);		 /* get memory from kernel */

	block_header_t *b = (block_header_t *)raw; /* cast */

	b->size = size;
	b->free = 0;
	b->next = g_head; /* insert at the head of the list */
	g_head = b;
	return (b);
}

/* round up to multiple of a... a squared? */
#define ROUNDUP(n, a) (((n) + (a) - 1) & ~((a) - 1))

/**
 * _malloc - allocates enough memory to store chunk header and size requested
 * @size: size needed to be allocated for the user
 *
 * This function imitates the behavior of malloc() by allocating memory
 * using sbrk() system call. It reserves a chunk of memory that is
 * aligned to the system page size and also aligned to the size of
 * max_align_t.
 *
 * Return: pointer to aligned allocated memory
 */
void *_malloc(size_t size)
{
	/* round up to the next multiple */
	size = ROUNDUP(size, alignof(max_align_t));

	/* check if size is too large */
	for (block_header_t *b = g_head; b; b = b->next)
	{
		if (b->free && b->size >= size) /* if free block found */
		{
			if (b->size > size + sizeof(block_header_t)) /* block too big */
			{
				/* split the block */
				block_header_t *new_block = (block_header_t *)((char *)(b + 1) + size);

				new_block->size = b->size - size - sizeof(block_header_t);
				new_block->free = 1;
				new_block->next = b->next;

				b->size = size;
				b->next = new_block;
			}
			b->free = 0;			  /* mark block as being used */
			return ((void *)(b + 1)); /* payload starts here */
		}
		/* if the block is exactly the size we need */
		else if (b->free && b->size == size)
		{
			b->free = 0;
			return ((void *)(b + 1)); /* similarly, payload starts here */
		}
	}

	/* nothing free — ask for more memory */
	block_header_t *blk = extend_heap(size);

	/* PROFIT */
	return (blk ? (void *)(blk + 1) : NULL);
}
