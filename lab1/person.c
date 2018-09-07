#include "person.h"

#include <stdbool.h>

char* getName(char **fields, int NF) {
    char *name = (char*) malloc(MAX_NAME_LENGTH*sizeof(char));
    name[0] = '\0';
    for (int i = 1; i < NF; i++)
    {
        char *tmp = (char*) strdup(fields[i]);
        if (i == NF - 1)
        {
            strcat(name, tmp);
        }
        else
        {
            strcat(name, tmp);
            strcat(name, " ");
        }
        free(tmp);
    }
    return name;
}

Person* new_person()
{
    Person *newPerson = (Person*) malloc(sizeof(Person));
    newPerson->name = (char*) malloc(MAX_NAME_LENGTH * sizeof(char));
    newPerson->name[0] = '\0';
    newPerson->father = (char*) malloc(MAX_NAME_LENGTH*sizeof(char));
    newPerson->father[0] = '\0';
    newPerson->mother = (char*) malloc(MAX_NAME_LENGTH*sizeof(char));
    newPerson->mother[0] = '\0';
    newPerson->children = new_dllist();
    newPerson->numChildren = 0;
    newPerson->sex = '\0';
    return newPerson;
}

Person* new_person_name(const char *pname)
{
    Person *newPerson = (Person*) malloc(sizeof(Person));
    newPerson->name = (char*) malloc(MAX_NAME_LENGTH);
    newPerson->name[0] = '\0';
    memcpy(newPerson->name, pname, strlen(pname)*sizeof(char));
    newPerson->father = (char*) malloc(MAX_NAME_LENGTH*sizeof(char));
    newPerson->father[0] = '\0';
    newPerson->mother = (char*) malloc(MAX_NAME_LENGTH*sizeof(char));
    newPerson->mother[0] = '\0';
    newPerson->children = new_dllist();
    newPerson->numChildren = 0;
    newPerson->sex = '\0';
    return newPerson;
}

Person* getChild(JRB people, Person *parent, char *cname)
{
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
    if (!hasChild)
    {
        perror("Error: that's not this person's child!");
        exit(-3);
    }
    Person *child;
    JRB node = jrb_find_str(people, cname);
    if (node == NULL)
    {
        child = new_person_name(cname);
        if (parent->sex == 'M')
        {
            memcpy(child->father, parent->name, strlen(parent->name));
            //child->father = parent->name;
        }
        else if (parent->sex == 'F')
        {
            memcpy(child->mother, parent->name, strlen(parent->name));
            //child->mother = parent->name;
        }
        (void*) jrb_insert_str(people, cname, new_jval_v((void*)child));
    }
    else
    {
        child = (Person*) node->val.v;
        //child = (Person*) jval_v(node->val);
    }
    return child;
}

void addChild(JRB people, Person *parent, char *cname)
{
    Dllist iter = dll_first(parent->children);
    Dllist nil = dll_nil(parent->children);
    while (iter != nil)
    {
        if (strcmp(((Person*) iter->val.v)->name, cname) == 0)
        {
            return;
        }
        iter = dll_next(iter);
    }
    Person *child;
    JRB node = jrb_find_str(people, cname);
    if (node == NULL)
    {
        child = new_person_name(cname);
        if (parent->sex == 'M')
        {
            memcpy(child->father, parent->name, strlen(parent->name));
            //child->father = parent->name;
        }
        else if (parent->sex == 'F')
        {
            memcpy(child->mother, parent->name, strlen(parent->name));
            //child->mother = parent->name;
        }
        (void*) jrb_insert_str(people, cname, new_jval_v((void*)child));
    }
    else
    {
        child = (Person*) node->val.v;
        if (parent->sex == 'M')
        {
            memcpy(child->father, parent->name, strlen(parent->name));
            //child->father = parent->name;
        }
        else if (parent->sex == 'F')
        {
            memcpy(child->mother, parent->name, strlen(parent->name));
            //child->mother = parent->name;
        }
    }
    dll_append(parent->children, new_jval_v((void*)child));
    parent->numChildren++;
}

Person* getFather(JRB people, Person *child)
{
    Person *father;
    if (child->father != NULL)
    {
        JRB node = jrb_find_str(people, child->father);
        if (node == NULL)
        {
            father = new_person_name(child->father);
            father->sex = 'M';
            addChild(people, father, child->name);
            (void*) jrb_insert_str(people, child->father, new_jval_v((void*)father));
        }
        else
        {
            father = (Person*) node->val.v;
            //father = (Person*) jval_v(node->val);
            if (father->sex != 'M')
            {
                father->sex = 'M';
            }
            addChild(people, father, child->name);
        }
        return father;
    }
    return NULL;
}

void setFather(JRB people, Person *child, char **fields, int NF)
{
    char *pname = getName(fields, NF);
    /*if (strcmp(child->father, pname) == 0)
    {
        return;
    }*/
    memcpy(child->father, pname, strlen(pname));
    //child->father = strdup(pname);
    (void*) getFather(people, child);
    free(pname);
}

Person* getMother(JRB people, Person *child)
{
    Person *mother;
    if (child->mother != NULL)
    {
        JRB node = jrb_find_str(people, child->mother);
        if (node == NULL)
        {
            mother = new_person_name(child->mother);
            mother->sex = 'F';
            addChild(people, mother, child->name);
            (void*) jrb_insert_str(people, child->mother, new_jval_v((void*)mother));
        }
        else
        {
            mother = (Person*) node->val.v;
            //mother = (Person*) jval_v(node->val);
            if (mother->sex != 'F')
            {
                mother->sex = 'F';
            }
            addChild(people, mother, child->name);
        }
        return mother;
    }
    return NULL;
}

void setMother(JRB people, Person *child, char **fields, int NF)
{
    char *pname = getName(fields, NF);
    /*if (strcmp(child->mother, pname) == 0)
    {
        return;
    }*/
    memcpy(child->mother, pname, strlen(pname));
    //child->mother = strdup(pname);
    (void*) getMother(people, child);
    free(pname);
}

void setSex(Person *p, char sex)
{
    p->sex = sex;
}

void destroyPerson(Person *p)
{
    free(p->name);
    free(p->father);
    free(p->mother);
    free(p->children);
    free(p);
}
