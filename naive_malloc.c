#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include "malloc.h"

/**
 * naive_malloc - Allocates enough memory to store chunk header and sz requested
 * @size: size needed to be allocated for the user
 *
 * Return: pointer to suitably aligned allocated memory
 */
void *naive_malloc(size_t size)
{
	size_t page_size = sysconf(_SC_PAGESIZE); /* system page size */
	size_t header_size = sizeof(size_t);      /* store chunk header */
	size_t total_request_size = header_size + size; /* total memory needed */
	size_t bytes_to_page_end; /* distance to end of page from current break */
	size_t current_break = (size_t)sbrk(0);  /* current location on page NOTE: possibly use uintptr_t instead of size_t */
	size_t page_start = current_break & ~(page_size - 1); /* memory addr of page start */
	size_t bytes_for_alignment; /* bytes needed to be word-aligned */
	size_t bytes_from_page_start; /* bytes away from page start */

	bytes_to_page_end = page_size - current_break;
	if (bytes_to_page_end < total_request_size) /*add enough padding to get to a new page */
	{
		sbrk(bytes_to_page_end);
		current_break = (size_t)sbrk(0);
	}

	/* align with required padding using sbrk(number of padding bytes) */
	bytes_from_page_start = page_size - bytes_to_page_end;
	bytes_for_alignment = bytes_from_page_start % sizeof(size_t);
	if (bytes_for_alignment)
		sbrk(bytes_for_alignment);

	/* reserve total request size sbrk(total_request_size) */
	sbrk(total_request_size);

	/* return pointer to user data (not the header) */
	return (void *)((char *)sbrk(0) - (ptrdiff_t)size);
	/* Profit */
}
