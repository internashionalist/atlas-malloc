#include "malloc.h"

/**
 * naive_malloc -	allocates enough memory to store chunk header
 *					and size requested
 * @size:			size needed to be allocated for the user
 *
 * This function imitates the behavior of malloc() by allocating memory
 * using sbrk.
 *
 * Return:			pointer to suitably aligned allocated memory
 */
void *naive_malloc(size_t size)
{
	size_t page_size = sysconf(_SC_PAGESIZE); /* system page size */
	size_t header_size = sizeof(size_t);      /* store chunk header */
	size_t total_request_size = header_size + size; /* total memory needed */
	size_t bytes_to_page_end; /* distance to end of page from current break */
	size_t current_break = (size_t)sbrk(0);  /* current location on page */
	/* NOTE: possibly use uintptr_t instead of size_t */
	/* UNUSED: size_t page_start = current_break & ~(page_size - 1); */
	/* memory addr of page start */
	size_t bytes_for_alignment; /* bytes needed to be word-aligned */
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

	/* add enough padding to get to a new page */
	if (bytes_for_alignment)
		sbrk(alignof(max_align_t) - bytes_for_alignment);

	/* reserve total request size sbrk(total_request_size) */
	sbrk(total_request_size);

	/* return pointer to user data (not the header) */
	return ((void *)((char *)sbrk(0) - (ptrdiff_t)size));

	/* Profit */
}
