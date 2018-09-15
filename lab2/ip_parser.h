#ifndef IP_PARSER_H
#define IP_PARSER_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jval.h"
#include "dllist.h"

inline void* _checkMalloc(void *ptr)
{
    if (ptr == NULL)
    {
        perror("Error: malloc failed");
        exit(-1);
    }
    return ptr;
}

#define memChk(p) _checkMalloc(p)

#define MAX_NAME_LENGTH 1024

typedef struct ip_data
{
    unsigned char address[4];
    Dllist names;
} IP;

extern IP* new_ip();

extern void read_bin_data(IP* ip, FILE *stream);

extern char* get_address(IP *ip);

extern void print_data(IP *ip, FILE *stream);

extern void free_ip(IP *ip);

#endif