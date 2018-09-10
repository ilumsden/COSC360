/* famtree.c
 * Author: Ian Lumsden
 *
 * Reads in a file, produces the corresponding tree, and prints the tree.
 */

#include "person.h"

// Prints the tree
void printFamtree(JRB people)
{
    Dllist queue = new_dllist();
    JRB tmp;
    JRB nil = jrb_nil(people);
    // Adds all people in the tree who don't have a parent to the queue.
    jrb_traverse(tmp, people)
    {
        if (tmp == nil)
        {
            continue;
        }
        if (tmp == NULL)
        {
            errno = EJRBTRAVERSE;
            perror("Internal Error: jrb_traverse produced a NULL pointer");
            exit(-1);
        }
        if (strcmp(((Person*)tmp->val.v)->father, "") == 0 && 
            strcmp(((Person*)tmp->val.v)->mother, "") == 0)
        {
            dll_append(queue, tmp->val);
        }
    }
    while (!dll_empty(queue))
    {
        // Pop a person off the front
        Person *p = (Person*) dll_first(queue)->val.v;
        dll_delete_node(dll_first(queue));
        // If the person has already been printed, do nothing.
        if (!p->printed)
        {
            // If the person doesn't have a parent or all the person's parent's have been
            // printed, move on to the next step.
            if ((strcmp(p->father, "") == 0 && strcmp(p->mother, "") == 0) ||
                (strcmp(p->father, "") == 0 && getMother(people, p, 0)->printed) ||
                (getFather(people, p, 0)->printed && strcmp(p->mother, "") == 0) ||
                (getFather(people, p, 0)->printed && getMother(people, p, 0)->printed))
            {
                // Print person
                printPerson(p);
                // Add the person's children to the list
                p->printed = 1;
                Dllist dtmp;
                Dllist dnil = dll_nil(p->children);
                dll_traverse(dtmp, p->children)
                {
                    if (dtmp == dnil)
                    {
                        continue;
                    }
                    if (dtmp == NULL)
                    {
                        errno = EDLLTRAVERSE;
                        perror("Internal Error: dll_traverse produced a NULL pointer.");
                        exit(-1);
                    }
                    dll_append(queue, dtmp->val);
                }
            }
        }
    }
    // Free the list's memory.
    free_dllist(queue);
}

int main(int argc, char **argv)
{
    // Red-Black Tree
    JRB people;
    Person *p = NULL;
    // Input parser
    IS is;
    // Makes the input parser by opening the file from argv[1]
    is = new_inputstruct(NULL);
    // Constructs the Red-Black tree
    people = make_jrb();
    // For each line in the file
    while (get_line(is) >= 0)
    {
        // If a blank line, do nothing
        if (is->NF == 0)
        {
            continue;
        }
        // If its a person line, extract/create the person
        else if (strcmp(is->fields[0], "PERSON") == 0)
        {
            char *name = getName(is->fields, is->NF);
            JRB node = jrb_find_str(people, name);
            if (node == NULL)
            {
                p = new_person_name(name);
                (void*) jrb_insert_str(people, p->name, new_jval_v((void*)p));
            }
            else
            {
                p = (Person*) node->val.v;
            }
            free(name);
        }
        // If its a sex line, set the person's sex
        else if (strcmp(is->fields[0], "SEX") == 0)
        {
            char sex = *(is->fields[1]);
            setSex(p, sex, is->line);
        }
        // If its a father line, set the person's father
        else if (strcmp(is->fields[0], "FATHER") == 0)
        {
            setFather(people, p, is->fields, is->NF, is->line);
        }
        // If its a mother line, set the person's mother
        else if (strcmp(is->fields[0], "MOTHER") == 0)
        {
            setMother(people, p, is->fields, is->NF, is->line);
        }
        // If its a father_of line, set the child
        else if (strcmp(is->fields[0], "FATHER_OF") == 0)
        {
            if (p->sex != 'M')
            {
                setSex(p, 'M', is->line);
            }
            char *cname = getName(is->fields, is->NF);
            addChild(people, p, cname);
            free(cname);
        }
        // If its a mother_of line, set the child
        else if (strcmp(is->fields[0], "MOTHER_OF") == 0)
        {
            if (p->sex != 'F')
            {
                setSex(p, 'F', is->line);
            }
            char *cname = getName(is->fields, is->NF);
            addChild(people, p, cname);
            free(cname);
        }
        // If the keyword is unrecognized, raise an error.
        else
        {
            fprintf(stderr, "3: Unknown key: %s\n", is->fields[0]);
            return -1;
        }
    }
    // Check for cycles
    cycleCheck(people);
    // Print the tree
    printFamtree(people);
    // Free data
    JRB per;
    jrb_rtraverse(per, people)
    {
        Person *elem = (Person*) per->val.v;
        destroyPerson(elem);
    }
    jrb_free_tree(people);
    jettison_inputstruct(is);
    return 0;
}
