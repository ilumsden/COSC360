#include "person.h"

int main(int argc, char **argv)
{
    // Red-Black Tree
    JRB people;
    Person *p = NULL;
    // Input parser
    IS is;
    // Makes the input parser by opening the file from argv[1]
    is = new_inputstruct(argv[1]);
    // Constructs the Red-Black tree
    people = make_jrb();
    while (get_line(is) >= 0)
    {
        if (strcmp(is->fields[0], "PERSON") == 0)
        {
            if (p != NULL)
            {
                JRB node = jrb_find_str(people, p->name);
                if (node == NULL)
                {
                    (void*) jrb_insert_str(people, p->name, new_jval_v((void*)p));
                }
            }
            char *name = getName(is->fields, is->NF);
            JRB node = jrb_find_str(people, name);
            if (node == NULL)
            {
                p = new_person_name(name);
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
            if (p->sex != sex)
            {
                setSex(p, sex);
            }
        }
        else if (strcmp(is->fields[0], "FATHER") == 0)
        {
            setFather(people, p, is->fields, is->NF);
        }
        else if (strcmp(is->fields[0], "MOTHER") == 0)
        {
            setMother(people, p, is->fields, is->NF);
        }
        else if (strcmp(is->fields[0], "FATHER_OF") == 0)
        {
            if (p->sex != 'M')
            {
                setSex(p, 'M');
            }
            char *cname = getName(is->fields, is->NF);
            addChild(people, p, cname);
            free(cname);
        }
        else if (strcmp(is->fields[0], "MOTHER_OF") == 0)
        {
            if (p->sex != 'F')
            {
                setSex(p, 'F');
            }
            char *cname = getName(is->fields, is->NF);
            addChild(people, p, cname);
            free(cname);
        }
        else
        {
            perror("Error: Invalid Specifier");
            return -1;
        }
    }
    JRB per;
    jrb_traverse(per, people)
    {
        Person *elem = (Person*) per->val.v;
        printf("PERSON %s\n", elem->name);
        printf("    SEX %c\n", elem->sex);
        printf("    FATHER %s\n", elem->father);
        printf("    MOTHER %s\n", elem->mother);
        Dllist iter = dll_first(((Person*)per->val.v)->children);
        Dllist nil = dll_nil(((Person*)per->val.v)->children);
        while (iter != nil)
        {
            printf("    CHILD %s\n", ((Person*)iter->val.v)->name);
            iter = dll_next(iter);
        }
    }
    jrb_rtraverse(per, people)
    {
        Person *elem = (Person*) per->val.v;
        jrb_delete_node(per);
        destroyPerson(elem);
    }
    if (jrb_empty(people))
    {
        jrb_free_tree(people);
    }
    else
    {
        perror("Error: people tree should be empty.");
        return -3;
    }
    jettison_inputstruct(is);
}
