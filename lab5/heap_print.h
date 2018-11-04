#ifndef HEAP_PRINT_H
#define HEAP_PRINT_H

/* heap_print.h
 * Author: Ian Lumsden
 *
 * This file contains the definitions of a set of functions used to print the contents of
 * memory to stdout. It is based on a similar set of printing code from Dr. Plank's Memory
 * lecture notes (web.eecs.utk.edu/~plank/plank/classes/cs360/360/notes/Memory/lecture.html).
 * Note that most of these functions use variable arguments to allow for user control of how
 * much printing is done.
 */

#include <stdarg.h>

// This typedef was from Dr. Plank's code.
typedef unsigned long int UI;
// This typedef represents a generic function pointer.
typedef void (*funcptr)(void);

// Used to access the end, etext, and edata variables.
extern char end;
extern char etext;
extern char edata;

// Prints the page size and the addresses of etext, edata, and end.
void print_initial();

// Prints the memory addresses of the passed function pointers
void print_funcs(char **func_names, int numfuncs,...);

// Prints the memory addresses of the passed global variables.
void print_global(char **gbl_names, int numgbl,...);

// Prints the memory addresses of the passed heap contents.
void print_heap(char **heap_names, int numheap,...);

// Prints the memory addresses of the passed stack variables.
void print_stack(char **stack_names, int numstack,...);

// Prints the contents of /proc/{pid}/maps
// NOTE: currently this function doesn't seem to work. The system() call currently
// always fails with a return value of -1. Since this is not a required part of the lab,
// it may not be corrected before it is turned in.
void print_sys(char *buf);

#endif
