#include "tree_creator.h"

static void getName(char *name, char **fields, int NF) {
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

static void makePerson(JRB *people, IS is, Person *person)
{
    bool encounteredPersonTag = false;
    do
    {
        if (strcmp(is->fields[0], "PERSON") == 0)
        {
            if (!encounteredPersonTag)
            {
                encounteredPersonTag = true;
                getName(person->name, is->fields, is->NF);
            }
            else
            {
                break;
            }
        }
        else if (strcmp(is->fields[0], "FATHER") == 0)
        {
            char *fatherName;
            getName(fatherName, is->fields, is->NF);
            Person *fatherPtr = (Person*) jrb_find_str(people, fatherName);
            if (fatherPtr == NULL)
            {
                fatherPtr = malloc(sizeof(Person));
                fatherPtr->name = fatherName;
                fatherPtr->sex = 'M';
                person->father = fatherPtr;
                (void) jrb_insert_str(people, fatherName, 
                                      new_jval_v((void*)fatherPtr));
            }
            else
            {
                if (fatherPtr->sex != 'M')
                {
                    fatherPtr->sex = 'M';
                }
            }
        }
        else if (strcmp(is->fields[0], "MOTHER") == 0)
        {
            char *motherName;
            getName(motherName, is->fields, is->NF);
            Person *motherPtr = (Person*) jrb_find_str(people, motherName);
            if (motherPtr == NULL)
            {
                motherPtr = malloc(sizeof(Person));
                motherPtr->name = motherName;
                motherPtr->sex = 'F';
                person->mother = motherPtr;
                (void) jrb_insert_str(people, motherName, 
                                      new_jval_v((void*)motherPtr));
            }
            else
            {
                if (motherPtr->sex != 'F')
                {
                    motherPtr->sex = 'F';
                }
            }
        }
        else if (strcmp(is->fields[0], "FATHER_OF") == 0)
        {
        
        }
        else if (strcmp(is->fields[0], "MOTHER_OF") == 0)
        {
        
        }
        else if (strcmp(is->fields[0], "SEX") == 0)
        {
        
        }
        else if (is->NF == 0)
        {
            break;
        }
        else
        {
            perror("Error: Unknown Parameter");
            exit(-2);
        }
    } while (get_line(is) >= 0);
}

int fillTree(JRB *people, IS is)
{
    int childInd = 0;
    bool subPerson = false;
    if (is == NULL)
    {
        perror("File Open Error: ");
        return -1;
    }
    while (get_line(is) >= 0)
    {
        if (strcmp(is->fields[0], "PERSON") == 0)
        {
            if (p != NULL)
            {
                (void) jrb_insert_str(people, p->name, new_jval_v((void*)p));
            }
            p = (Person *)malloc(sizeof(Person));
            p->children = (char **)malloc(MAX_CHILDREN * sizeof(char *));
            getName(p->name, is->fields, is->NF);
        }
        else
        {
            switch (is->fields[0])
            {
                case "SEX":
                    p->sex = is->fields[1];
                    break;
                case "FATHER":
                    p->father = is->fields[1];
                    break;
                case "MOTHER":
                    p->mother = is->fields[1];
                    break;
                case "FATHER_OF" || "MOTHER_OF":
                    char *name;
                    getName(name, is->fields, is->NF);
                    p->children[childInd] = name;
                    break;
                default:
                    perror("Error: Unknown Parameter");
                    return -2;
            }
        }
    }
}
