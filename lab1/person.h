#ifndef PERSON_H
#define PERSON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jrb.h"
#include "jval.h"
#include "fields.h"

#define INIT_CHILDREN 10
#define MAX_NAME_LENGTH 1024

void getName(char *name, char **fields, int NF);

typedef struct person_t
{
    char  *name;
    char   sex;
    char  *father;
    char  *mother;
    char **children;
    int    numChildren;
    int    maxChildren;
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

#endif
