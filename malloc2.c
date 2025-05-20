#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include "malloc.h"
#include <stdalign.h> /* alignof(max_align_t) */

/* --- simple free‑list block header ------------------------------- */
typedef struct block_header
{
    size_t size;               /* payload size (bytes)        */
    int    free;               /* 1 = available, 0 = in use   */
    struct block_header *next; /* singly‑linked list          */
} block_header_t;

/* Head of the global block list (first‑fit order) */
static block_header_t *g_head = NULL;

/**
 * _malloc - Allocates enough memory to store chunk header and sz requested
 * @size: size needed to be allocated for the user
 *
 * This function imitates the behavior of malloc() by allocating memory
 * using sbrk() system call. It reserves a chunk of memory that is
 * aligned to the system page size and also aligned to the size of
 * max_align_t.
 *
 * Return: pointer to suitably aligned allocated memory
 */
void *_malloc(size_t size)
{
	/* Round size up to satisfy worst‑case alignment */
	size = (size + alignof(max_align_t) - 1) & ~(alignof(max_align_t) - 1);

	/* First‑fit: reuse the first sufficiently large free block */
	for (block_header_t *blk = g_head; blk; blk = blk->next)
	{
		if (blk->free && blk->size >= size)
		{
			blk->free = 0;
			return (void *)(blk + 1); /* payload starts after header */
		}
	}

	size_t page_size = sysconf(_SC_PAGESIZE);		/* system page size */
	size_t header_size = sizeof(block_header_t);	/* struct header */
	size_t total_request_size = header_size + size; /* total memory needed */
	/* distance to end of page from current break */
	size_t bytes_to_page_end;
	size_t current_break = (size_t)sbrk(0);			/* current location on page */
	/* mem addr of page start */
	/* size_t page_start = current_break & ~(page_size - 1); */
	size_t bytes_for_alignment;	  /* bytes needed to be word-aligned */
	size_t bytes_from_page_start; /* bytes away from page start */

	bytes_to_page_end = page_size - current_break;
	/*add enough padding to get to a new page */
	if (bytes_to_page_end < total_request_size)
	{
		sbrk(bytes_to_page_end);
		current_break = (size_t)sbrk(0);
	}

	/* align with required padding using sbrk(number of padding bytes) */
	bytes_from_page_start = page_size - bytes_to_page_end;
	bytes_for_alignment = bytes_from_page_start % alignof(max_align_t);
	if (bytes_for_alignment)
		sbrk(alignof(max_align_t) - bytes_for_alignment);

	/* reserve total request size sbrk(total_request_size) */
	void *block_start = sbrk(total_request_size);
	if (block_start == (void *)-1)
		return NULL;

	/* set up header */
	block_header_t *new_block = (block_header_t *)block_start;
	new_block->size = size;
	new_block->free = 0;
	new_block->next = g_head; /* prepend for quick first‑fit */
	g_head = new_block;

	/* return pointer to user data (not the header) */
	return (void *)(new_block + 1);

	/* Profit */
}
