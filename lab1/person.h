#ifndef PERSON_H
#define PERSON_H

// Added to maintain the definition of POSIX functions in C99
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dllist.h"
#include "jrb.h"
#include "jval.h"
#include "fields.h"

#define MAX_NAME_LENGTH 128

extern char* getName(char **fields, int NF);

typedef struct person_t
{
    char  *name;
    char   sex;
    char  *father;
    char  *mother;
    Dllist children;
    int    numChildren;
} Person;

extern Person* new_person();

extern Person* new_person_name(const char *pname);

extern Person* getChild(JRB people, Person *parent, char *cname);

extern void addChild(JRB people, Person *parent, char *cname);

extern Person* getFather(JRB people, Person *child);

extern void setFather(JRB people, Person *child, char **fields, int NF);

extern Person* getMother(JRB people, Person *child);

extern void setMother(JRB people, Person *child, char **fields, int NF);

extern void setSex(Person *p, char sex);

extern void destroyPerson(Person *p);

#endif
