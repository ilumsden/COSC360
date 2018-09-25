#include "compile.h"
#include "fields.h"

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        fprintf(stderr, "No target. Stopping.\n");
        return -1;
    }
    else if (strcmp(argv[1], "clean") == 0)
    {
        if ( system("rm *.o") < 0 )
        {
            fprintf(stderr, "Could not delete .o files.\n");
            return -1;
        }
        return 0;
    }
    IS is = new_inputstruct(argv[1]);
    Compile *compilation = new_compilation();
    while (get_line(is) >= 0)
    {
        if (is->NF == 0)
        {
            continue;
        }
        else if (strcmp(is->fields[0], "E") == 0)
        {
            add_exec(compilation, is->fields, is->NF);
        }
        else if (strcmp(is->fields[0], "C") == 0)
        {
            add_srcs(compilation, is->fields, is->NF);
        }
        else if (strcmp(is->fields[0], "H") == 0)
        {
            add_headers(compilation, is->fields, is->NF);
        }
        else if (strcmp(is->fields[0], "F") == 0)
        {
            add_flags(compilation, is->fields, is->NF);
        }
        else if (strcmp(is->fields[0], "L") == 0)
        {
            add_libraries(compilation, is->fields, is->NF);
        }
        else
        {
            fprintf(stderr, "Error: invalid line (%d)\n", is->line);
            return -1;
        }
    }
    char **commands = get_commands(compilation);
    for (int i = 0; i < compilation->num_commands; i++)
    {
        printf("%s\n", commands[i]);
        if ( system(commands[i]) < 0 )
        {
            fprintf(stderr, "Compilation failed\n");
            return -1;
        }
    }
    for (int i = 0; i < compilation->num_commands; i++)
    {
        free(commands[i]);
    }
    free(commands);
    free_compilation(compilation);
    jettison_inputstruct(is);
}
