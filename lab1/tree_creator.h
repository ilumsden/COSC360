#ifndef TREE_CREATOR_H
#define TREE_CREATOR_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jrb.h"
#include "jval.h"
#include "fields.h"

#define MAX_CHILDREN 20
#define MAX_NAME_LENGTH 1024

typedef struct
{
    char  *name;
    char   sex;
    Person  *father;
    Person  *mother;
    Person **children;
} Person;

void getName(char *name, char **fields, int NF);

void makePerson(JRB *people, IS is, Person *person);

void fillTree(JRB *people, IS is);

#endif
