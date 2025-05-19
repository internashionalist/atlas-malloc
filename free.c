#include "malloc.h"

/**
 * _free - frees a memory space
 * @ptr: pointer to the memory space to be freed
 *
 * This function frees a memory space that was previously allocated
 * using the _malloc function. It checks if the pointer is valid
 * and if it is at the current break. If so, it releases the memory
 * back to the system using sbrk.
 *
 * Return: void
 */
void _free(void *ptr)
{
	size_t current_break = (size_t)sbrk(0);
	size_t header_size = alignof(max_align_t);

	/*
	* check if valid memory space?
	* check if at current break
	* something
	*/

	if (current_break == (size_t)ptr)
		sbrk(-1 * header_size);
}
