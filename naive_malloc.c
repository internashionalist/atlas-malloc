#include "malloc.h"

/**
 * naive_malloc - Allocates enough memory to store chunk header and sz requested
 * @size: size needed to be allocated for the user
 *
 * Return: pointer to suitably aligned allocated memory
 */
void *naive_malloc(size_t size)
{
	size_t page_size = sysconf(_SC_PAGESIZE);
	size_t header_size = 16; // or sizeof(long double)?;
	size_t total_request_size = header_size + size;
	size_t current_program_break = sbrk(0);

	if (page_size - (current_program_break % page_size) < 0)
	{
		//add enough padding to get to a new page
	}

	//align with required padding using sbrk(number of padding bytes)
	//reserve total request size sbrk(total_request_size)
	//return pointer to user data (not the header)

	//monetize with a memecoin
}
