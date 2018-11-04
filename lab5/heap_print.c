#include "heap_print.h"
#include <stdlib.h>

void print_initial()
{
    printf("Page size: %d\n", getpagesize());
    printf("\n");
    printf("&etext: 0x%lx\n", (UI)&etext);
    printf("&edata: 0x%lx\n", (UI)&edata);
    printf("&end: 0x%lx\n", (UI)&end);
    printf("\n");
}

void print_funcs(char **func_names, int numfuncs,...)
{
    va_list valist;
    va_start(valist, numfuncs);
    printf("Code Addresses:\n");
    for (int i = 0; i < numfuncs; i++)
    {
        printf("%s: 0x%lx\n", func_names[i], (UI)(va_arg(valist, funcptr)));
    }
    va_end(valist);
    printf("\n");
}

void print_global(char **gbl_names, int numgbl,...)
{
    va_list valist;
    va_start(valist, numgbl);
    printf("Global Variable Addresses:\n");
    for (int i = 0; i < numgbl; i++)
    {
        printf("%s: 0x%lx\n", gbl_names[i], (UI)(va_arg(valist, void*)));
    }
    va_end(valist);
    printf("\n");
}

void print_heap(char **heap_names, int numheap,...)
{
    va_list valist;
    va_start(valist, numheap);
    printf("Heap Addresses:\n");
    for (int i = 0; i < numheap; i++)
    {
        printf("%s: 0x%lx\n", heap_names[i], (UI)(va_arg(valist, void*)));
    }
    va_end(valist);
    printf("\n");
}

void print_stack(char **stack_names, int numstack,...)
{
    va_list valist;
    va_start(valist, numstack);
    printf("Stack Addresses:\n");
    for (int i = 0; i < numstack; i++)
    {
        printf("&%s: 0x%lx\n", stack_names[i], (UI)(va_arg(valist, void*)));
    }
    va_end(valist);
    printf("\n");
}

void print_sys(char *buf)
{
    sprintf(buf, "cat /proc/%d/maps", getpid());
    system(buf);
}
