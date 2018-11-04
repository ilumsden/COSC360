#ifndef FREELIST_H
#define FREELIST_H

/* freelist.h
 * Author: Ian Lumsden
 *
 * This file contains the declarations of the MALLOC_BUF_SIZE macro, the Flist struct, and
 * the functions associated with the Flist struct. The functions contain the underlying logic
 * used in my "malloc" functions.
 */

// Added to make sbrk work
#define _XOPEN_SOURCE 500
#define _DEFAULT_SOURCE

#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// The size of the sbrk buffer obtained
#define MALLOC_BUF_SIZE INT_MAX

// This struct defines the data needed for an element of the free list.
typedef struct fnode_t
{
    unsigned int size;
    struct fnode_t *flink;
    struct fnode_t *blink;
} *Flist;

// Uses sbrk to get a new buffer with a default size and link pointers.
Flist create_flist();

// Creates a checksum for a block of memory returned from malloc functions.
unsigned int calc_checksum(unsigned int size);

// Gets a block from the free list of size "size"
void* get_block(Flist head, unsigned int size);

// Looks for a block of memory that can be used to contiguously resize the current malloc pointer
// to newsize.
void* search_for_extra_space(Flist head, void *currptr, unsigned int currsize, 
        unsigned int newsize);

// Returns the block of malloced memory to the free list.
void return_block(Flist head, void *ptr);

#endif
