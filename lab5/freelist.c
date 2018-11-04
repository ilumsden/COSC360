/* freelist.c
 * Author: Ian Lumsden
 *
 * Implementations of the functions defined in freelist.h
 */

#include "freelist.h"

Flist create_flist()
{
    // Uses sbrk to get a new memory block, sets its size and links to default values, and
    // returns the block casted to a Flist.
    Flist buf = (Flist) sbrk((intptr_t) MALLOC_BUF_SIZE);
    buf->size = MALLOC_BUF_SIZE;
    buf->flink = NULL;
    buf->blink = NULL;
    return buf;
}

unsigned int calc_checksum(unsigned int size)
{
    // Produces and returns a checksum for a block of malloced data by XORing the bytes 
    // representing the size of the block.
    char *bytes = (char*) &size;
    unsigned int checksum = (unsigned int) bytes[0];
    for (int i = 1; i < 4; i++)
    {
        checksum = checksum ^ (unsigned int) bytes[1];
    }
    return checksum;
}

void* get_block(Flist head, unsigned int size)
{
    // Calculates the 8-byte-alligned size of the requested memory block
    unsigned int full_size = (size % 8 == 0) ? size : size + (8 - (size % 8));
    // Adds space for the bookkeeping bytes to the size.
    full_size += 8;
    // Because this is developed for a 64-bit architecture, the size of an Flist object
    // is 20 bytes. As a result, for a chunk of malloced memory to be able to be returned
    // to the free list, it must have a size of 20 or more. Since malloced memory must be
    // alligned by 8, the minimum malloced block size is 24.
    if (full_size < 24)
    {
        full_size = 24;
    }
    // Loops through the free list elements
    Flist currbuf = head;
    while (currbuf != NULL)
    {
        // Does nothing if the current buffer is not big enough for the requested size
        if (currbuf->size >= full_size)
        {
            // If the data is large enough that, after getting the requested data, a new 
            // Flist object can be created, the requested memory is removed and handed to the user,
            // and the remainder is placed back into the free list as an Flist object.
            if (currbuf->size - full_size >= 24)
            {
                if (currbuf->flink != NULL)
                {
                    currbuf->flink->blink = currbuf->blink;
                }
                if (currbuf->blink != NULL)
                {
                    currbuf->blink->flink = currbuf->flink;
                }
                currbuf->blink = NULL;
                currbuf->flink = NULL;
                currbuf->size = 0;
                char *splitbuf = (char*) currbuf;
                // Gets the memory that's going to be put back on the free list, and
                // adds it to the front of the list.
                Flist putback = (Flist) (splitbuf + full_size);
                putback->flink = head;
                head->blink = putback;
                head = putback;
                unsigned int *intbuf = (unsigned int*) splitbuf;
                *intbuf = full_size - 8;
                char *retbuf = (char*) intbuf;
                intbuf = (unsigned int*) (retbuf + 4);
                *intbuf = calc_checksum(full_size - 8);
                retbuf = (char*) intbuf;
                return (void*) (retbuf + 4);
            }
            // If the remaining memory after slicing the requested data is not large enough to
            // create a new Flist object, all the memory in the current free list element is 
            // returned to the user.
            else if (currbuf->size - full_size >= 0)
            {
                if (currbuf->flink != NULL)
                {
                    currbuf->flink->blink = currbuf->blink;
                }
                if (currbuf->blink != NULL)
                {
                    currbuf->blink->flink = currbuf->flink;
                }
                currbuf->blink = NULL;
                currbuf->flink = NULL;
                currbuf->size = 0;
                unsigned int *intbuf = (unsigned int*) currbuf;
                *intbuf = full_size - 8;
                char *retbuf = (char*) intbuf;
                intbuf = (unsigned int*) (retbuf + 4);
                *intbuf = calc_checksum(full_size - 8);
                retbuf = (char*) intbuf;
                return (void*) (retbuf + 4);
            }
        }
        // Advances to the next element.
        currbuf = currbuf->flink;
    }
    // If no free list element can provide the requested data, a new one is created.
    Flist newblock = create_flist();
    unsigned int initsize = newblock->size;
    newblock->size = 0;
    // The requested memory is sliced off the new free list block, and the remainder of the
    // block is added to the free list.
    char *split = (char*) newblock;
    Flist returnblock = (Flist) (split + full_size);
    returnblock->size = initsize - full_size;
    if (head != NULL)
    {
        returnblock->flink = head;
        head->blink = returnblock;
    }
    head = returnblock;
    unsigned int *buf = (unsigned int*) split;
    *buf = full_size - 8;
    char *newbuf = (char*) buf;
    buf = (unsigned int*) (newbuf + 4);
    *buf = calc_checksum(full_size - 8);
    newbuf = (char*) buf;
    return (void*) (newbuf + 4);
}

void* search_for_extra_space(Flist head, void *currptr, unsigned int currsize, 
                             unsigned int new_size)
{
    // Calculates the 8-byte-alligned size of the requested memory block
    unsigned int size = (new_size % 8 == 0) ? new_size : new_size + (8 - (new_size % 8));
    // Calculates the difference between the old size and the new size.
    unsigned int diff = size - currsize;
    // Loops through the free list elements
    Flist curr = head;
    while (curr != NULL)
    {
        // If the current element is not contiguous with currptr, move to the next element.
        if (curr == currptr + currsize)
        {
            // If the current element does not have enough memory to increase currptr's size to
            // new_size, get a new memory block from the free list. Then, copy the memory from
            // currptr to the new block, and return currptr's memory to the free list.
            if (curr->size < diff)
            {
                void *tmp_block = get_block(head, new_size);
                memcpy(tmp_block, currptr, currsize);
                return_block(head, currptr);
                return tmp_block;
            }
            // See get_block.
            else if (curr->size - diff >= 24)
            {
                if (curr->flink != NULL)
                {
                    curr->flink->blink = curr->blink;
                }
                if (curr->blink != NULL)
                {
                    curr->blink->flink = curr->flink;
                }
                curr->blink = NULL;
                curr->flink = NULL;
                curr->size = 0;
                char *splitbuf = (char*) curr;
                // Gets the memory that's going to be put back on the free list, and
                // adds it to the front of the list.
                Flist putback = (Flist) (splitbuf + diff);
                putback->flink = head;
                head->blink = putback;
                head = putback;
                char *charptr = (char*) currptr;
                unsigned int *intptr = (unsigned int*) (charptr - 8);
                *intptr = size;
                charptr = (char*) intptr;
                return (void *) (charptr + 8);
            }
            // See get_block
            else if (curr->size - diff >= 0)
            {
                if (curr->flink != NULL)
                {
                    curr->flink->blink = curr->blink;
                }
                if (curr->blink != NULL)
                {
                    curr->blink->flink = curr->flink;
                }
                curr->blink = NULL;
                curr->flink = NULL;
                curr->size = 0;
                char *charptr = (char*) currptr;
                unsigned int *intptr = (unsigned int*) (charptr - 8);
                *intptr = size;
                charptr = (char*) intptr;
                return (void *) (charptr + 8);
            }
        }
        // Advances to the next element.
        curr = curr->flink;
    }
    // If no free list element is contiguous with currptr, get a new memory block 
    // from the free list. Then, copy the memory from currptr to the new block, 
    // and return currptr's memory to the free list.
    void *fin_block = get_block(head, new_size);
    memcpy(fin_block, currptr, currsize);
    return_block(head, currptr);
    return fin_block;
}

void return_block(Flist head, void *ptr)
{
    // If NULL is passed as the pointer, do nothing.
    if (head == NULL || ptr == NULL)
    {
        return;
    }
    // Get the size and checksum from the bookkeeping bytes.
    char *bookptr = (char*) ptr;
    unsigned int checksum = (unsigned int) *(bookptr - 4);
    unsigned int size = (unsigned int) *(bookptr - 8);
    // Calculate the checksum from the bookkeeping bytes.
    unsigned int calced = calc_checksum(size);
    // If the calculated checksum doesn't match the bookkeeping checkusm, raise an error
    // and exit the program.
    // Note: the _exit system call is used to avoid issues with stdlib.h
    if (calced != checksum)
    {
        fprintf(stderr, "Error: invalid checksum for freed block.\n");
        _exit(1);
    }
    // Reset all bytes in the block.
    for (int i = -8; i < (int) size; i++)
    {
        *(bookptr + i) = 0;
    }
    // Convert the block to an Flist and set the data accordingly.
    Flist buf = (Flist) (bookptr - 8);
    buf->size = size + 8;
    if (head != NULL)
    {
        buf->flink = head;
        head->blink = buf;
    }
    head = buf;
    return;
}
