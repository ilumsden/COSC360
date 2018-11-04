#ifndef _MALLOC_H
#define _MALLOC_H

/* malloc.h
 * 
 * This file was provided by the professor. It is a simple header file used to define my 
 * implementation of the malloc functions.
 */

#ifdef __cplusplus
extern "C" {
#endif

void *malloc(unsigned int size);
void *calloc(unsigned int nmemb, unsigned int size);
void *realloc(void *ptr, unsigned int size);
void free(void *ptr);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif
