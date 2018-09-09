#ifndef PERSON_H
#define PERSON_H

// Added to maintain the definition of POSIX functions in C99
#define _POSIX_C_SOURCE 200809L

#include <errno.h>

#define EBADCHILD 150 /* Child doesn't belong to parent. */
#define EBADFATHER 151 /* Person is not the child's father. */
#define EMULTIPARENT 152 /* Person cannot be a mother and father at same time. */
#define EBADMOTHER 153 /* Person is not the child's mother. */
#define EGENDERFORCE 154 /* Person's sex is already defined, so it can't be changed */
#define EJRBTRAVERSE 155 /* JRB traverse produced a NULL pointer. */
#define EDLLTRAVERSE 156 /* Dllist traverse produced a NULL pointer. */
#define EDESCENDANTCYCLE 157 /* There's a cycle in the tree. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dllist.h"
#include "jrb.h"
#include "jval.h"
#include "fields.h"

inline void* _checkMalloc(void *ptr)
{
    if (ptr == NULL)
    {
        perror("Error: malloc failed");
        exit(-5);
    }
    return ptr;
}

#define memChk(p) _checkMalloc(p)

#define MAX_NAME_LENGTH 1024

extern char* getName(char **fields, int NF);

typedef struct person_t
{
    char  *name;
    char   sex;
    char  *father;
    char  *mother;
    Dllist children;
    int    numChildren;
    int    visited;
    int    printed;
} Person;

extern Person* new_person();

extern Person* new_person_name(const char *pname);

extern Person* getChild(JRB people, Person *parent, char *cname);

extern void addChild(JRB people, Person *parent, char *cname);

extern Person* getFather(JRB people, Person *child, int iline);

extern void setFather(JRB people, Person *child, char **fields, int NF, int iline);

extern Person* getMother(JRB people, Person *child, int iline);

extern void setMother(JRB people, Person *child, char **fields, int NF, int iline);

extern void setSex(Person *p, char sex, int iline);

extern void printPerson(Person *p);

extern void destroyPerson(Person *p);

extern int isDescendant(Person *p);

extern void cycleCheck(JRB people);

#endif
