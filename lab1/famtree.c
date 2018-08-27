#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "jrb.h"
#include "fields.h"

typedef struct
{
    char  *name;
    char   sex;
    char  *father;
    char  *mother;
    char **children;
} Person;

int main(int argc, char **argv)
{
    // Red-Black Tree
    JRB people;
    Person *p;
    // Input parser
    IS is;
    // Makes the input parser by opening the file from argv[1]
    is = new_inputstruct(argv[1]);
    // Constructs the Red-Black tree
    people = make_jrb();
    bool subPerson = false;
    if (is == NULL)
    {
        perror("File Open Error: ");
        return -1;
    }
    while (get_line(is) >= 0)
    {
        switch (is->
    }
}
