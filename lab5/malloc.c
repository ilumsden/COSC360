/* malloc.c
 * Author: Ian Lumsden
 *
 * This file contains my implementations of the 4 malloc-related functions.
 */

#include "malloc.h"
#include "freelist.h"

// A pointer to the head of the free list.
static Flist flist_head = NULL;

// A wrapper function for the get_block function from freelist.c/h
void* malloc(unsigned int size)
{
    return get_block(flist_head, size);
}

// Same as malloc, but with explicit initialization.
// Note: with modern compilers, this acts the exact same as malloc because modern
// compilers automatically initialize data.
void* calloc(unsigned int nmemb, unsigned int size)
{
    // Grabs the memory block using (effectively) malloc
    void *buf = get_block(flist_head, nmemb*size);
    char *cbuf = (char*) buf;
    // Initializes all the bytes of the memory block to 0
    for (int i = 0; i < (int) nmemb; i++)
    {
        for (int j = 0; j < (int) size; j++)
        {
            cbuf[size*i + j] = 0;
        }
    }
    // Returns the initialized memory block.
    return (void*) cbuf;
}

// Adds extra memory to the passed memory block, or, if that's not possible, returns
// a new memory block of the desired size with the contents of the old block copied over.
void* realloc(void *ptr, unsigned int size)
{
    char *cptr = (char*) ptr;
    // Extracts the current size from the memory block's bookkeeping bytes.
    unsigned int currsize = (unsigned int) *(cptr-8);
    if (size < currsize)
    {
        return ptr;
    }
    // Hands off the rest of the logic to search_for_extra_space from freelist.c/h
    return search_for_extra_space(flist_head, ptr, currsize, size);
}

// A wrapper function for return_block from freelist.c/h
void free(void *ptr)
{
    return_block(flist_head, ptr);
}
