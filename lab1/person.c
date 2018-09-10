/* person.c
 * Author: Ian Lumsden
 *
 * Implementations of the functions defined in person.h
 */

#include "person.h"

#include <stdbool.h>

char* getName(char **fields, int NF) {
    // Allocates the memory for the name
    char *name = (char*) memChk(malloc(MAX_NAME_LENGTH*sizeof(char)));
    // Sets the initial value
    name[0] = 0;
    // Loops through the fields
    for (int i = 1; i < NF; i++)
    {
        // Gets the field
        char *tmp = (char*) strdup(fields[i]);
        // If there is only one name, copy it to name
        if (i == 1 && i == NF - 1)
        {
            strcpy(name, tmp);
        }
        // If there are multiple names, copy the first one and add a space.
        else if (i == 1)
        {
            strcpy(name, tmp);
            strcat(name, " ");
        }
        // If on the last name, just concatenate witout the space.
        else if (i == NF - 1)
        {
            strcat(name, tmp);
        }
        // Otherwise, concatenate the name and add a space.
        else
        {
            strcat(name, tmp);
            strcat(name, " ");
        }
        // Free the temporary string
        free(tmp);
    }
    return name;
}

Person* new_person()
{
    // Allocates the person
    Person *newPerson = (Person*) memChk(malloc(sizeof(Person)));
    // Allocates the name
    newPerson->name = (char*) memChk(malloc(MAX_NAME_LENGTH * sizeof(char)));
    newPerson->name[0] = 0;
    // Allocates the father
    newPerson->father = (char*) memChk(malloc(MAX_NAME_LENGTH*sizeof(char)));
    newPerson->father[0] = 0;
    // Allocates the mother
    newPerson->mother = (char*) memChk(malloc(MAX_NAME_LENGTH*sizeof(char)));
    newPerson->mother[0] = 0;
    // Sets up the children
    newPerson->children = new_dllist();
    // Set remaining data
    newPerson->numChildren = 0;
    newPerson->sex = 0;
    newPerson->visited = 0;
    newPerson->printed = 0;
    return newPerson;
}

Person* new_person_name(const char *pname)
{
    // Set up the default person
    Person* newPerson = new_person();
    // Set name
    strcpy(newPerson->name, pname);
    return newPerson;
}

Person* getChild(JRB people, Person *parent, char *cname)
{
    // Check if the child is in the parent's child list
    Dllist iter = dll_first(parent->children);
    Dllist nil = dll_nil(parent->children);
    bool hasChild = false;
    while (iter != nil)
    {
        if (strcmp(((Person*)iter->val.v)->name, cname) == 0)
        {
            hasChild = true;
            break;
        }
        iter = dll_next(iter);
    }
    // If child is not in the list, raise an error.
    if (!hasChild)
    {
        errno = EBADCHILD;
        perror("Error: that's not this person's child!");
        exit(-1);
    }
    // Extracts the child
    Person *child;
    JRB node = jrb_find_str(people, cname);
    if (node == NULL)
    {
        child = new_person_name(cname);
        if (parent->sex == 'M')
        {
            strcpy(child->father, parent->name);
        }
        else if (parent->sex == 'F')
        {
            strcpy(child->mother, parent->name);
        }
        (void*) jrb_insert_str(people, child->name, new_jval_v((void*)child));
    }
    else
    {
        child = (Person*) node->val.v;
    }
    return child;
}

void addChild(JRB people, Person *parent, char *cname)
{
    // Checks if the child is already present
    Dllist iter = dll_first(parent->children);
    Dllist nil = dll_nil(parent->children);
    while (iter != nil)
    {
        // If already present, return.
        if (strcmp(((Person*) iter->val.v)->name, cname) == 0)
        {
            return;
        }
        iter = dll_next(iter);
    }
    // Extract the child object
    Person *child;
    JRB node = jrb_find_str(people, cname);
    if (node == NULL)
    {
        child = new_person_name(cname);
        if (parent->sex == 'M')
        {
            strcpy(child->father, parent->name);
        }
        else if (parent->sex == 'F')
        {
            strcpy(child->mother, parent->name);
        }
        (void*) jrb_insert_str(people, child->name, new_jval_v((void*)child));
    }
    else
    {
        child = (Person*) node->val.v;
        if (parent->sex == 'M')
        {
            strcpy(child->father, parent->name);
        }
        else if (parent->sex == 'F')
        {
            strcpy(child->mother, parent->name);
        }
    }
    // Add the child to the list
    dll_append(parent->children, new_jval_v((void*)child));
    // Increment numChildren
    parent->numChildren++;
}

Person* getFather(JRB people, Person *child, int iline)
{
    // Extracts the father object and add the child.
    Person *father;
    if (strcmp(child->father, "") != 0)
    {
        JRB node = jrb_find_str(people, child->father);
        if (node == NULL)
        {
            father = new_person_name(child->father);
            setSex(father, 'M', iline);
            addChild(people, father, child->name);
            (void*) jrb_insert_str(people, child->father, new_jval_v((void*)father));
        }
        else
        {
            father = (Person*) node->val.v;
            if (father->sex != 'M')
            {
                setSex(father, 'M', iline);
            }
            addChild(people, father, child->name);
        }
        // Returns the father
        return father;
    }
    // Otherwise returns NULL
    return NULL;
}

void setFather(JRB people, Person *child, char **fields, int NF, int iline)
{
    // Gets the father's name
    char *pname = getName(fields, NF);
    // If the father's name doesn't match the current father, raise an error.
    if (strcmp(child->father, "") != 0 && strcmp(child->father, pname) != 0)
    {
        errno = EBADFATHER;
        fprintf(stderr, "Bad input -- child with two fathers on line %d\n", iline);
        exit(-1);
    }
    // Use getFather to finish the rest.
    strcpy(child->father, pname);
    (void*) getFather(people, child, iline);
    free(pname);
}

Person* getMother(JRB people, Person *child, int iline)
{
    // Extracts the mother object and add the child.
    Person *mother;
    if (strcmp(child->mother, "") != 0)
    {
        JRB node = jrb_find_str(people, child->mother);
        if (node == NULL)
        {
            mother = new_person_name(child->mother);
            setSex(mother, 'F', iline);
            addChild(people, mother, child->name);
            (void*) jrb_insert_str(people, child->mother, new_jval_v((void*)mother));
        }
        else
        {
            mother = (Person*) node->val.v;
            if (mother->sex != 'F')
            {
                setSex(mother, 'F', iline);
            }
            addChild(people, mother, child->name);
        }
        // Returns the mother
        return mother;
    }
    // Otherwise returns NULL
    return NULL;
}

void setMother(JRB people, Person *child, char **fields, int NF, int iline)
{
    // Gets the mother's name
    char *pname = getName(fields, NF);
    // If the mother's name doesn't match the current mother, raise an error.
    if (strcmp(child->mother, "") != 0 && strcmp(child->mother, pname) != 0)
    {
        errno = EBADMOTHER;
        fprintf(stderr, "Bad input -- child with two mothers on line %d\n", iline);
        exit(-1);
    }
    // Use getMother to finish the rest.
    strcpy(child->mother, pname);
    (void*) getMother(people, child, iline);
    free(pname);
}

void setSex(Person *p, char sex, int iline)
{
    // If the sex is unset, set it.
    if (p->sex == 0)
    {
        p->sex = sex;
    }
    // Otherwise, if the provided sex doesn't match the current, raise an error.
    else
    {
        if (p->sex != sex)
        {
            errno = EGENDERFORCE;
            fprintf(stderr, "Bad input - sex mismatch on line %d\n", iline);
            exit(-1);
        }
    }
}

void printPerson(Person *p)
{
    // Prints name
    printf("%s\n", p->name);
    // Prints sex
    if (p->sex == 'M')
    {
        printf("  Sex: Male\n");
    }
    else if (p->sex == 'F')
    {
        printf("  Sex: Female\n");
    }
    else
    {
        printf("  Sex: Unknown\n");
    }
    // Prints father name
    if (strcmp(p->father, "") == 0)
    {
        printf("  Father: Unknown\n");
    }
    else
    {
        printf("  Father: %s\n", p->father);
    }
    // Prints mother name
    if (strcmp(p->mother, "") == 0)
    {
        printf("  Mother: Unknown\n");
    }
    else
    {
        printf("  Mother: %s\n", p->mother);
    }
    Dllist iter = dll_first(p->children);
    Dllist nil = dll_nil(p->children);
    // If there are no children, print none.
    if (iter == nil)
    {
        printf("  Children: None\n");
    }
    // Else print the children name
    else
    {
        printf("  Children:\n");
        while (iter != nil)
        {
            printf("    %s\n", ((Person*)iter->val.v)->name);
            iter = dll_next(iter);
        }
    }
    printf("\n");
}

void destroyPerson(Person *p)
{
    // Frees all the pointers in the person
    free(p->name);
    free(p->father);
    free(p->mother);
    free_dllist(p->children);
    free(p);
}

int isDescendant(Person *p)
{
    // If the person has already been successfully visited, return 0.
    if (p->visited == 1) return 0;
    // If the person has already been unsuccessfully visited, return 1.
    if (p->visited == 2) return 1;
    p->visited = 2;
    // If the recursive function call ever returns true, return 1.
    Dllist tmp;
    dll_traverse(tmp, p->children)
    {
        if (isDescendant((Person*) tmp->val.v)) return 1;
    }
    // Set visited to 0 and return 0.
    p->visited = 1;
    return 0;
}

void cycleCheck(JRB people)
{
    JRB nil = jrb_nil(people);
    JRB tmp;
    jrb_traverse(tmp, people)
    {
        // If tmp is root, continue.
        if (tmp == nil)
        {
            continue;
        }
        // If tmp is NULL, raise an error.
        if (tmp == NULL)
        {
            errno = EJRBTRAVERSE;
            perror("Internal Error: jrb_traverse produced a NULL pointer");
            exit(-1);
        }
        // If isDescendant returns 1 for the current person, raise an error.
        Person *person = (Person*) tmp->val.v;
        if (isDescendant(person))
        {
            errno = EDESCENDANTCYCLE;
            fprintf(stderr, "Bad input -- cycle in specification\n");
            exit(-1);
        }
    }
}
