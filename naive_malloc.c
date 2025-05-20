#include <stddef.h>   /* for size_t */
#include <unistd.h>   /* for sbrk, sysconf */
#include "malloc.h"

#define DEFAULT_PAGE 4096

static void   *region_base;		/* start of reserved bump region */
static size_t  region_size;		/* size of each block (header+payload) */
static size_t  region_used;		/* bytes already handed out */

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
	void			*prev_end;		/* previous end of allocated region */
	void			*ptr;			/* pointer to payload */
	size_t			aligned_size;	/* aligned size */
	long			pg;				/* system page size */

	if (size == 0)					/* check for zero size */
		return (NULL);

	pg = sysconf(_SC_PAGESIZE);		/* figure out page size */
	if (pg <= 0)					/* if error, use default */
		pg = DEFAULT_PAGE;

    /* round up requested + header to a page boundary */
	aligned_size = (size + sizeof(size_t) + pg - 1) & ~(pg - 1);

    /* on first call, reserve the bump region */
	if (region_base == NULL)
	{
		region_size = aligned_size;	/* block size */
		region_base = sbrk(0);		/* current break */
		if (sbrk(region_size) == (void *)-1)	/* if sbrk fails */
			return (NULL);
    }

    /* carve next block */
	prev_end = (char *)region_base + region_used; /* gives end of region */
	region_used += region_size;						/* update used size */

	*(size_t *)prev_end = region_size;			/* store block size header */

    /* return payload past header */
	ptr = (char *)prev_end + sizeof(size_t);

	return (ptr);					/* return pointer to payload */
}
