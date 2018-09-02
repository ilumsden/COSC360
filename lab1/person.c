#include "person.h"

void getName(char *name, char **fields, int NF) {
    for (int i = 1; i < NF; i++)
    {
        if (i == 1)
        {
            name = fields[i];
        }
        else if (i == NF - 1)
        {
            strcat(name, fields[i]);
        }
        else
        {
            strcat(name, fields[i]);
            strcat(name, " ");
        }
    }
}

Person* new_person()
{
    Person *newPerson = (Person*) malloc(sizeof(Person));
    newPerson->children = (char**) malloc(INIT_CHILDREN * sizeof(char*));
    newPerson->numChildren = 0;
    newPerson->maxChildren = INIT_CHILDREN;
    return newPerson;
}

Person* new_person_name(const char *pname)
{
    Person *newPerson = (Person*) malloc(sizeof(Person));
    newPerson->name = pname;
    newPerson->children = (char**) malloc(INIT_CHILDREN * sizeof(char*));
    newPerson->numChildren = 0;
    newPerson->maxChildren = INIT_CHILDREN;
    return newPerson;
}

Person* getChild(JRB people, Person *parent, char *cname)
{
    for (int i = 0; i < parent->numChildren; i++)
    {
        if (strcmp(parent->children[i], cname) != 0)
        {
            perror("Error: that's not this person's child!");
            exit(-3);
        }
    }
    Person *child;
    JRB node = jrb_find_str(peopel, cname);
    if (node == NULL)
    {
        child = new_person_name(cname);
        if (parent->sex == 'M')
        {
            child->father = parent->name;
        }
        else if (parent->sex == 'F')
        {
            child->mother = parent->name;
        }
        (void*) jrb_insert_str(people, cname, new_jval_v((void*)child));
    }
    else
    {
        child = (Person*) jval_v(node->val);
    }
    return child;
}

void addChild(JRB people, Person *parent, char *cname)
{
    for (int i = 0; i < parent->numChildren; i++)
    {
        if (strcmp(parent->children[i], cname) == 0)
        {
            return;
        }
    }
    JRB node = jrb_find_str(people, cname);
    if (node == NULL)
    {
        Person *child = new_person_name(cname);
        if (parent->sex == 'M')
        {
            child->father = parent->name;
        }
        else if (parent->sex == 'F')
        {
            child->mother = parent->name;
        }
        (void*) jrb_insert_str(people, cname, new_jval_v((void*)child));
    }
    if (parent->numChildren == parent->maxChildren)
    {
        char **tmp = (char**) malloc(parent->numChildren * sizeof(char*));
        memcpy(tmp, parent->children, parent->numChildren*sizeof(char*));
        parent->maxChildren *= 5;
        parent->children = (char**) malloc(parent->maxChildren*sizeof(char*));
        memcpy(parent->children, tmp, parent->numChildren*sizeof(char*));
    }
    parent->children[parent->numChildren] = cname;
    parent->numChildren++;
}

Person* getFather(JRB people, Person *child)
{
    Person *father
    if (child->father != NULL)
    {
        JRB node = jrb_find_str(people, child->father);
        if (node == NULL)
        {
            father = new_person_name(child->father);
            father->sex = 'M';
            addChild(father, child->name);
            (void*) jrb_insert_str(people, child->father, new_jval_v((void*)father));
        }
        else
        {
            father = (Person*) jval_v(node->val);
            if (father->sex != 'M')
            {
                father->sex = 'M';
            }
            addChild(father, child->name);
        }
        return father;
    }
    return NULL;
}

void setFather(JRB people, Person *child, char **fields, int NF)
{
    char *pname;
    getName(pname, fields, NF);
    if (strcmp(child->father, pname) == 0)
    {
        return;
    }
    child->father = pname;
    (void*) getFather(people, child);
}

Person* getMother(JRB people, Person *child)
{
    Person *mother
    if (child->mother != NULL)
    {
        JRB node = jrb_find_str(people, child->mother);
        if (node == NULL)
        {
            mother = new_person_name(child->mother);
            mother->sex = 'F';
            addChild(mother, child->name);
            (void*) jrb_insert_str(people, child->mother, new_jval_v((void*)mother));
        }
        else
        {
            mother = (Person*) jval_v(node->val);
            if (mother->sex != 'F')
            {
                mother->sex = 'F';
            }
            addChild(mother, child->name);
        }
        return mother;
    }
    return NULL;
}

void setMother(JRB people, Person *child, char **fields, int NF)
{
    char *pname;
    getName(pname, fields, NF);
    if (strcmp(child->mother, pname) == 0)
    {
        return;
    }
    child->mother = pname;
    (void*) getMother(people, child);
}
