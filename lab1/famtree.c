#include "person.h"

void printFamtree(JRB people)
{
    Dllist queue = new_dllist();
    JRB tmp;
    JRB nil = jrb_nil(people);
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
        Person *p = (Person*) dll_first(queue)->val.v;
        dll_delete_node(dll_first(queue));
        if (!p->printed)
        {
            if ((strcmp(p->father, "") == 0 && strcmp(p->mother, "") == 0) ||
                (strcmp(p->father, "") == 0 && getMother(people, p, 0)->printed) ||
                (getFather(people, p, 0)->printed && strcmp(p->mother, "") == 0) ||
                (getFather(people, p, 0)->printed && getMother(people, p, 0)->printed))
            {
                printPerson(p);
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
    while (get_line(is) >= 0)
    {
        if (is->NF == 0)
        {
            continue;
        }
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
                //p = (Person*) jval_v(node->val);
                p = (Person*) node->val.v;
            }
            free(name);
        }
        else if (strcmp(is->fields[0], "SEX") == 0)
        {
            char sex = *(is->fields[1]);
            setSex(p, sex, is->line);
        }
        else if (strcmp(is->fields[0], "FATHER") == 0)
        {
            setFather(people, p, is->fields, is->NF, is->line);
        }
        else if (strcmp(is->fields[0], "MOTHER") == 0)
        {
            setMother(people, p, is->fields, is->NF, is->line);
        }
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
        else
        {
            fprintf(stderr, "3: Unknown key: %s\n", is->fields[0]);
            return -1;
        }
    }
    cycleCheck(people);
    /*JRB per;
    JRB nil = jrb_nil(people);
    jrb_traverse(per, people)
    {
        if (per == nil)
        {
            continue;
        }
        Person *elem = (Person*) per->val.v;
        printf("%s\n", elem->name);
        if (elem->sex == 'M')
        {
            printf("  Sex: Male\n");
        }
        else if (elem->sex == 'F')
        {
            printf("  Sex: Female\n");
        }
        else
        {
            printf("  Sex: Unknown\n");
        }
        if (strcmp(elem->father, "") == 0)
        {
            printf("  Father: Unknown\n");
        }
        else
        {
            printf("  Father: %s\n", elem->father);
        }
        if (strcmp(elem->mother, "") == 0)
        {
            printf("  Mother: Unknown\n");
        }
        else
        {
            printf("  Mother: %s\n", elem->mother);
        }
        Dllist iter = dll_first(((Person*)per->val.v)->children);
        Dllist nil = dll_nil(((Person*)per->val.v)->children);
        if (iter == nil)
        {
            printf("  Children: None\n");
        }
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
    }*/
    printFamtree(people);
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
