#include "freelist.h"

Flist create_flist()
{
    Flist buf = (Flist) sbrk((intptr_t) MALLOC_BUF_SIZE);
    buf->size = MALLOC_BUF_SIZE;
    buf->flink = NULL;
    buf->blink = NULL;
    return buf;
}

unsigned int calc_checksum(unsigned int size)
{
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
    unsigned int full_size = (size % 8 == 0) ? size : size + (8 - (size % 8));
    full_size += 8;
    // Because this is developed for a 64-bit architecture, the size of an Flist object
    // is 20 bytes. As a result, for a chunk of malloced memory to be able to be returned
    // to the free list, it must have a size of 20 or more. Since malloced memory must be
    // alligned by 8, the minimum malloced block size is 24.
    if (full_size < 24)
    {
        full_size = 24;
    }
    Flist currbuf = head;
    while (currbuf != NULL)
    {
        if (currbuf->size >= full_size)
        {
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
        currbuf = currbuf->flink;
    }
    Flist newblock = create_flist();
    unsigned int initsize = newblock->size;
    newblock->size = 0;
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

void* search_for_extra_space(Flist head, void *currptr, unsigned int currsize, unsigned int new_size)
{
    Flist curr = head;
    while (curr != NULL)
    {
        if (curr == currptr + currsize)
        {
            unsigned int size = (new_size % 8 == 0) ? new_size : new_size + (8 - (new_size % 8));
            unsigned int diff = size - currsize;
            if (curr->size < diff)
            {
                return_block(head, currptr);
                return get_block(head, new_size);
            }
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
    }
    return_block(head, currptr);
    return get_block(head, new_size);
}

void return_block(Flist head, void *ptr)
{
    if (head == NULL || ptr == NULL)
    {
        return;
    }
    char *bookptr = (char*) ptr;
    unsigned int checksum = (unsigned int) *(bookptr - 4);
    unsigned int size = (unsigned int) *(bookptr - 8);
    unsigned int calced = calc_checksum(size);
    if (calced != checksum)
    {
        fprintf(stderr, "Error: invalid checksum for freed block.\n");
        _exit(1);
    }
    for (int i = -8; i < (int) size; i++)
    {
        *(bookptr + i) = 0;
    }
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
