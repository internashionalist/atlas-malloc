#include "malloc.h"

/**
 * _free - frees a memory space
 * @ptr: pointer to the memory space to be freed
 *
 * T
 *
 * Return: void
 */
void _free(void *ptr)
{
	if (!ptr) /* check if pointer is NULL */
		return;

	block_header_t *blk = (block_header_t *)ptr - 1; /* get the header */

	blk->free = 1; /* mark the block as free */

	/* merge with previous block if free */
	if (blk->next && blk->next->free)
	{
		/* merge with next block */
		blk->size += sizeof(block_header_t) + blk->next->size;
		/* advance the pointer */
		blk->next = blk->next->next;
	}
}
