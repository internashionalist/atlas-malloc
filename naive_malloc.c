#include <stddef.h>   /* for size_t */
#include <unistd.h>   /* for sbrk, sysconf */
#include "malloc.h"

/**
 * naive_malloc -	allocates memory in heap using sbrk
 * @size:			size needed to be allocated
 *
 * This function imitates the behavior of malloc() by allocating memory
 * using sbrk. A chunk of memory is reserved that is aligned to the
 * system page size.
 *
 * Return: pointer to payload, otherwise NULL
 */
void *naive_malloc(size_t size)
{
	static void		*heap_end;		/* end of allocated region */
	void			*prev_end;		/* previous end of allocated region */
	void			*ptr;			/* pointer to payload */
	size_t			aligned_size;	/* aligned size */
	long			pg;				/* system page size */

	if (size == 0)					/* check for zero size */
		return (NULL);

	pg = sysconf(_SC_PAGESIZE);		/* figure out page size */
	if (pg <= 0)					/* if error, use default */
		pg = 4096;

    /* round up requested + header to a page boundary */
	aligned_size = (size + sizeof(size_t) + pg - 1) & ~(pg - 1);

	if (heap_end == NULL)			/* if first call */
		heap_end = sbrk(0);			/* get current break */

	prev_end = heap_end;			/* get previous end of heap */

    /* extend the heap */
	if (sbrk(aligned_size) == (void *)-1)		/* if sbrk fails */
		return (NULL);
	heap_end = (char *)heap_end + aligned_size;	/* update end of heap */

    /* store block size header */
	*(size_t *)prev_end = aligned_size;			/* store size in header */

    /* return payload past header */
	ptr = (char *)prev_end + sizeof(size_t);

	return (ptr);					/* return pointer to payload */
}
