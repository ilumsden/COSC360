#include <stdarg.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

typedef unsigned long int UI;
typedef void (*funcptr)(void);

extern char end;
extern char etext;
extern char edata;

void print_initial();

void print_funcs(char **func_names, int numfuncs,...);

void print_global(char **gbl_names, int numgbl,...);

void print_heap(char **heap_names, int numheap,...);

void print_stack(char **stack_names, int numstack,...);

void print_sys(char *buf);
