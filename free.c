#include "malloc.h"

/**
 * _free - frees a memory space
 */
void _free(void *ptr)
{
	size_t current_break = (size_t)sbrk(0);
	size_t header_size = alignof(max_align_t);

	//check if valid memory space?
	//check if at current break
	//something

	if (current_break == (size_t)ptr)
		sbrk(-1 * header_size);
}
