#include <stddef.h>   /* for size_t */
#include <unistd.h>   /* for sbrk, sysconf */
#include "malloc.h"

# define ALIGNMENT sizeof(size_t)    /* alignment boundary */

static void   *region_base;		/* start of reserved bump region */
static size_t  region_size;		/* size of each block (header+payload) */
static size_t  region_used;		/* bytes already handed out */
static size_t  region_reserved;  /* total reserved region size */

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
	void			*payload;		/* pointer to payload */
	size_t			aligned_size;	/* aligned size of requested block */

	if (size == 0)					/* check for zero size */
		return (NULL);

    /* round up requested + header to an alignment boundary */
	aligned_size = (size + sizeof(size_t) + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
	/* sync with system page size */
	region_size = aligned_size;

	/* use page size if region size too small */
	long page = sysconf(_SC_PAGESIZE);

	if (page <= 0)
		page = 4096;
	region_reserved = page;

    /* on first call, reserve the bump region */
	if (region_base == NULL)
		region_base = sbrk(0);						/* current break */

    /* carve next block */
	prev_end = (char *)region_base + region_used;	/* gives end of region */
	region_used += region_size;						/* update used size */

	*(size_t *)prev_end = region_size;			/* store block size header */

    /* set payload pointer to first byte after the header */
	payload = (char *)prev_end + sizeof(size_t);

	/* return pointer to payload */
	return (payload);
}
