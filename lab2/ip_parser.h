#ifndef IP_PARSER_H
#define IP_PARSER_H

/* ip_parser.h
 * Author: Ian Lumsden
 *
 * Header file that defines an IP struct for storing an IP address and its names.
 * Also contains functions for the IP struct and support functions.
 */

// Added to maintain the definition of POSIX functions in C99
#define _POSIX_C_SOURCE 200809L

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "jval.h"
#include "dllist.h"
#include "jrb.h"

// Basic inline function to test if malloc was sucessfull
inline void* _checkMalloc(void *ptr)
{
    if (ptr == NULL)
    {
        perror("Error: malloc failed");
        exit(-1);
    }
    return ptr;
}

// This macro wraps _checkMalloc
#define memChk(p) _checkMalloc(p)

// The max length of a name
#define MAX_NAME_LENGTH 1024

// Concatenates the two ints into a new returned int
extern unsigned int intcat(unsigned int a, unsigned int b);

// This struct stores all the needed data for the IP
typedef struct ip_data
{
    unsigned char address_nums[4];
    char *address;
    int num_names;
    JRB names;
} IP;

// Creates a new, empty IP struct
extern IP* new_ip();

// Fills the IP address field (string) using the contents of address_nums
extern void gen_address(IP *ip);

// Creates an IP struct using data read from the FILE pointer.
extern void read_bin_data_fpointer(IP* ip, FILE *stream);

// Creates an IP struct using data read from the POSIX file descriptor
extern void read_bin_data_sys(IP* ip, int stream);

// Creates an IP struct using data from the provided buffer at current_loc
extern void read_bin_data_buf(IP* ip, char *buf, int *current_loc);

// Prints the data for the provided IP struct to the provided stream
extern void print_data(IP *ip, FILE *stream);

// Frees the memory for the IP struct
extern void free_ip(IP *ip);

#endif
