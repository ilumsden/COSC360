#define _XOPEN_SOURCE 500
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <unistd.h>

#define MALLOC_BUF_SIZE 8192

typedef struct fnode_t
{
    unsigned int size;
    struct fnode_t *flink;
    struct fnode_t *blink;
} *Flist;

Flist create_flist();

unsigned int calc_checksum(unsigned int size);

void* get_block(Flist head, unsigned int size);

void* search_for_extra_space(Flist head, void *currptr, unsigned int currsize, unsigned int newsize);

void return_block(Flist head, void *ptr);
