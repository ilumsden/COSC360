#include "../malloc.h"
#include "../heap_print.h"
#include <stdio.h>
#include <string.h>

int main()
{
    float i = 5;
    float c = (float)(int)'t';
    float p = 3.14159;
    char **func_names = (char**) malloc(5*sizeof(char*));
    func_names[0] = (char*) malloc(4);
    func_names[0] = "main";
    func_names[1] = (char*) malloc(6);
    func_names[1] = "malloc";
    func_names[2] = (char*) malloc(6);
    func_names[2] = "calloc";
    func_names[3] = (char*) malloc(7);
    func_names[3] = "realloc";
    func_names[4] = (char*) malloc(4);
    func_names[4] = "free";
    char **stack_names = (char**) calloc(3, sizeof(float));
    stack_names[0] = (char*) malloc(1);
    stack_names[0] = "i";
    stack_names[1] = (char*) malloc(1);
    stack_names[1] = "c";
    stack_names[2] = (char*) malloc(1);
    stack_names[2] = "p";
    char *buf = (char*) malloc(200);
    char **heap_names = (char**) malloc(10*sizeof(char*));
    heap_names[0] = (char*) malloc(10);
    heap_names[0] = "func_names";
    heap_names[1] = (char*) malloc(13);
    heap_names[1] = "func_names[0]";
    heap_names[2] = (char*) malloc(13);
    heap_names[2] = "func_names[1]";
    heap_names[3] = (char*) malloc(13);
    heap_names[3] = "func_names[2]";
    heap_names[4] = (char*) malloc(13);
    heap_names[4] = "func_names[3]";
    heap_names[5] = (char*) malloc(13);
    heap_names[5] = "func_names[4]";
    heap_names[6] = (char*) malloc(11);
    heap_names[6] = "stack_names";
    heap_names[7] = (char*) malloc(14);
    heap_names[7] = "stack_names[0]";
    heap_names[8] = (char*) malloc(13);
    heap_names[8] = "func_names[1]";
    heap_names[9] = (char*) malloc(13);
    heap_names[9] = "func_names[2]";
    heap_names = (char**) realloc(heap_names, 12*sizeof(char*));
    heap_names[10] = (char*) malloc(3);
    heap_names[10] = "buf";
    heap_names[11] = (char*) malloc(10);
    heap_names[11] = "heap_names";
    print_initial();
    print_funcs(func_names, 5, main, malloc, calloc, realloc, free);
    print_heap(heap_names, 12, func_names, func_names[0], func_names[1], func_names[2], func_names[3], func_names[4], 
                               stack_names, stack_names[0], stack_names[1], stack_names[2], buf, heap_names);
    print_stack(stack_names, 3, &i, &c, &p);
    print_sys(buf);
    for (int i = 0; i < 5; i++)
    {
        free(func_names[i]);
    }
    free(func_names);
    for (int i = 0; i < 3; i++)
    {
        free(stack_names[i]);
    }
    free(stack_names);
    for (int i = 0; i < 12; i++)
    {
        free(heap_names[i]);
    }
    free(heap_names);
    free(buf);
}
