#ifndef PERSON_H
#define PERSON_H

/* person.h
 * Author: Ian Lumsden
 *
 * Header file for anything person related.
 */

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

// Include for the Lab 5 Demo
#include "../malloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dllist.h"
#include "jrb.h"
#include "jval.h"
#include "fields.h"

/* This function simply ensures that malloc is successful. */
inline void* _checkMalloc(void *ptr)
{
    if (ptr == NULL)
    {
        perror("Error: malloc failed");
        exit(-5);
    }
    return ptr;
}

// This macro wraps _checkMalloc
#define memChk(p) _checkMalloc(p)

// The max length of a name
#define MAX_NAME_LENGTH 1024

// Gets the name from the fields and NF data from IS
extern char* getName(char **fields, int NF);

// This struct stores all the needed data for a person
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

// Creates a new, unnamed person
extern Person* new_person();

// Creates a new person with the provided name
extern Person* new_person_name(const char *pname);

// Gets the child with name cname if its the parent's child
extern Person* getChild(JRB people, Person *parent, char *cname);

// Adds the child with name cname to the parent's children list
extern void addChild(JRB people, Person *parent, char *cname);

// Gets the father of the current person 
extern Person* getFather(JRB people, Person *child, int iline);

// Set the father of the current person
extern void setFather(JRB people, Person *child, char **fields, int NF, int iline);

// Gets the mother of the current person
extern Person* getMother(JRB people, Person *child, int iline);

// Sets the mother of the current person
extern void setMother(JRB people, Person *child, char **fields, int NF, int iline);

// Sets the sex of the current person
extern void setSex(Person *p, char sex, int iline);

// Prints the information for the person
extern void printPerson(Person *p);

// Frees the memory for the person
extern void destroyPerson(Person *p);

// Checks if a person is their own descendant
extern int isDescendant(Person *p);

// Checks for cycles in the tree
extern void cycleCheck(JRB people);

#endif
