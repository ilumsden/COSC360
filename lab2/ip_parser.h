#ifndef IP_PARSER_H
#define IP_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jval.h"
#include "dllist.h"

#define MAX_NAME_LENGTH 1024

typedef struct ip_data
{
    unsigned char address[4];
    Dllist names;
} IP;

extern IP* new_ip();

extern void read_bin_data(IP* ip, FILE *stream);

extern void print_data(IP *ip, FILE *stream);

extern void free_ip(IP *ip);

#endif
