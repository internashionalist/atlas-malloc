#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include "malloc.h"
#include <stdalign.h> /* for alignof(max_align_t) */

/* okay so the struct here is a bit of a mess, but it works */
typedef struct block_header
{
    size_t size;				/* payload size (not including header *) */
    int free;					/* (1) if this block is available */
    struct block_header *next;	/* singly‑linked list */
} block_header_t;

static block_header_t *g_head = NULL; /* head of block list */

/**
 * _malloc -	allocates enough memory to store chunk header and size
 * @size:		size needed to be allocated for the user
 *
 * This function imitates the behavior of malloc() by allocating memory
 * using sbrk() system call. It reserves a chunk of memory that is
 * aligned to the system page size and also aligned to the size of
 * max_align_t.
 *
 * Return: pointer to aligned allocated memory
 */
void *_malloc(size_t size)
{
    /* align request size to size of max_align_t */
    size = (size + alignof(max_align_t) - 1) & ~(alignof(max_align_t) - 1);

    /* check if size is 0 */
    block_header_t *curr = g_head;
    while (curr)
    {
        if (curr->free && curr->size >= size)
        {
            curr->free = 0;				/* mark as used */
            return (void *)(curr + 1);	/* user payload starts after header */
        }
        curr = curr->next;
    }

    size_t page_size = sysconf(_SC_PAGESIZE);	    /* system page size */
    size_t header_size = sizeof(block_header_t); /* store chunk head struct */
    size_t total_request_size = header_size + size;	/* total memory needed */

    /* distance to end of page from current break */
    size_t bytes_to_page_end;
    size_t current_break = (size_t)sbrk(0);			/* current location on page */

    /* size_t page_start = current_break & ~(page_size - 1); */
    size_t bytes_for_alignment;			/* bytes needed to be word-aligned */
    size_t bytes_from_page_start;		/* bytes away from page start */

    bytes_to_page_end = page_size - current_break;

    /* add enough padding to get to a new page */
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

    /* reserve total_request_size bytes (header + payload) */
    void *block_start = sbrk(total_request_size);
    if (block_start == (void *)-1)
        return NULL;

    /* initialise block header */
    block_header_t *new_block = (block_header_t *)block_start;
    new_block->size = size;
    new_block->free = 0;

    /* prepend to linked list */
    new_block->next = g_head;
    g_head = new_block;

    /* return pointer to user data (not the header) */
    return (void *)(new_block + 1);

    /* Profit */
}
