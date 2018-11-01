#include "../jmalloc.h"
#include <stdio.h>

int main()
{
    int **arr = (int**) jcalloc(10, sizeof(int*));
    printf("Returned pointer: 0x%p\n", arr);
    printf("Bookkeeping Start: 0x%p\n", ((char*)arr - 8));
    for (int i = 0; i < 10; i++)
    {
        printf("%d: %p\n", i, arr[i]);
    }
    arr = (int**) jrealloc(arr, 15*sizeof(int*));
    printf("Returned pointer: 0x%p\n", arr);
    printf("Bookkeeping Start: 0x%p\n", ((char*)arr - 8));
    for (int i = 0; i < 15; i++)
    {
        printf("%d: %p\n", i, arr[i]);
    }
    jfree(arr);
    return 0;
}
