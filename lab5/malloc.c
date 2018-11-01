#include "malloc.h"
#include "freelist.h"

static Flist flist_head = NULL;

void* jmalloc(unsigned int size)
{
    return get_block(flist_head, size);
}

void* jcalloc(unsigned int nmemb, unsigned int size)
{
    void *buf = get_block(flist_head, nmemb*size);
    char *cbuf = (char*) buf;
    for (int i = 0; i < (int) nmemb; i++)
    {
        for (int j = 0; j < (int) size; j++)
        {
            cbuf[size*i + j] = 0;
        }
    }
    return (void*) cbuf;
}

void* jrealloc(void *ptr, unsigned int size)
{
    char *cptr = (char*) ptr;
    unsigned int currsize = (unsigned int) *(cptr-8);
    return search_for_extra_space(flist_head, ptr, currsize, size);
}

void jfree(void *ptr)
{
    return_block(flist_head, ptr);
}
