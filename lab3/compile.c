#include "compile.h"

Compile* new_compilation()
{
    Compile *compilation = (Compile*) malloc(sizeof(Compile));
    compilation->exec_name = (char*) malloc(MAX_EXEC_NAME);
    compilation->exec_name[0] = 0;
    compilation->srcs = (char**) malloc(MAX_ARGS*sizeof(char*));
    compilation->num_srcs = 0;
    compilation->headers = (char**) malloc(MAX_ARGS*sizeof(char*));
    compilation->num_headers = 0;
    compilation->flag_list = (char**) malloc(MAX_ARGS*sizeof(char*));
    compilation->num_flags = 0;
    compilation->libraries = (char**) malloc(MAX_ARGS*sizeof(char*));
    compilation->num_libs = 0;
    return compilation;
}

void add_srcs(Compile *compilation, char **fields, int NF)
{
    if (NF == 0)
    {
        fprintf(stderr, "Warning: Read in an empty line as a sources line.\n");
    }
    else if (strcmp(fields[0], "C") != 0)
    {
        fprintf(stderr, "Warning: Read in a non-source line as a source line.\n");
    }
    else
    {
        for (int i = 1; i < NF; i++)
        {
            char *file = fields[i];
            if (file[strlen(file)-2] != '.' && file[strlen(file)-1] != 'c')
            {
                fprintf(stderr, "Warning: Non-source file included on a sources line. Skipping.\n");
            }
            else
            {
                compilation->srcs[compilation->num_srcs] = strdup(file);
                compilation->num_srcs++;
                if (compilation->num_srcs == MAX_ARGS)
                {
                    fprintf(stderr, "Warning: Maximum number of sources reached (%d)\n", MAX_ARGS);
                    break;
                }
            }
        }
    }
}

void add_exec(Compile *compilation, char **fields, int NF)
{
    if (strcmp(compilation->exec_name, "") != 0)
    {
        fprintf(stderr, "Error: multiple executable names in the description file.\n");
        exit(-1);
    }
    else if (NF == 0)
    {
        fprintf(stderr, "Warning: Read in an empty line as an executable line.\n");
    }
    else if (strcmp(fields[0], "E") != 0)
    {
        fprintf(stderr, "Warning: Read in a non-executable line as an executable line.\n");
    }
    else if (NF == 2)
    {
        strcpy(compilation->exec_name, fields[1]);
    }
}

void add_headers(Compile *compilation, char **fields, int NF)
{
    if (NF == 0)
    {
        fprintf(stderr, "Warning: Read in an empty line as a header line.\n");
    }
    else if (strcmp(fields[0], "H") != 0)
    {
        fprintf(stderr, "Warning: Read in a non-header line as a header line.\n");
    }
    else
    {
        for (int i = 1; i < NF; i++)
        {
            char *file = fields[i];
            if (file[strlen(file)-2] != '.' && file[strlen(file)-1] != 'h')
            {
                fprintf(stderr, "Warning: Non-header file included on a header line. Skipping.\n");
            }
            else
            {
                compilation->headers[compilation->num_headers] = strdup(file);
                compilation->num_headers++;
                if (compilation->num_headers == MAX_ARGS)
                {
                    fprintf(stderr, "Warning: Maximum number of headers reached (%d)\n", MAX_ARGS);
                    break;
                }
            }
        }
    }
}

void add_flags(Compile *compilation, char **fields, int NF)
{
    if (NF == 0)
    {
        fprintf(stderr, "Warning: Read in an empty line as a flag line.\n");
    }
    else if (strcmp(fields[0], "F") != 0)
    {
        fprintf(stderr, "Warning: Read in a non-flag line as a flag line.\n");
    }
    else
    {
        for (int i = 1; i < NF; i++)
        {
            char *file = fields[i];
            if (file[0] != '-')
            {
                fprintf(stderr, "Warning: Non-flag included on a flag line. Skipping.\n");
            }
            else
            {
                compilation->flag_list[compilation->num_flags] = strdup(file);
                compilation->num_flags++;
                if (compilation->num_flags == MAX_ARGS)
                {
                    fprintf(stderr, "Warning: Maximum number of flags reached (%d)\n", MAX_ARGS);
                    break;
                }
            }
        }
    }
}

void add_libraries(Compile *compilation, char **fields, int NF)
{
    if (NF == 0)
    {
        fprintf(stderr, "Warning: Read in an empty line as a library line.\n");
    }
    else if (strcmp(fields[0], "L") != 0)
    {
        fprintf(stderr, "Warning: Read in a non-library line as a library line.\n");
    }
    else
    {
        for (int i = 1; i < NF; i++)
        {
            char *file = fields[i];
            if (strcmp(&file[strlen(file)-2], ".a") != 0 
                || strcmp(&file[strlen(file)-3], ".so") != 0)
            {
                fprintf(stderr, "Warning: Non-library included on a library line. Skipping.\n");
            }
            else
            {
                compilation->libraries[compilation->num_libs] = strdup(file);
                compilation->num_libs++;
                if (compilation->num_libs == MAX_ARGS)
                {
                    fprintf(stderr, "Warning: Maximum number of libraries reached (%d)\n", MAX_ARGS);
                    break;
                }
            }
        }
    }
}

char** get_commands(Compile *compilation)
{

}

void free_compilation(Compile *compilation)
{
    free(compilation->exec_name);
    for (int i = 0; i < compilation->num_srcs; i++)
    {
        free(compilation->srcs[i]);
    }
    free(compilation->srcs);
    for (int i = 0; i < compilation->num_headers; i++)
    {
        free(compilation->headers[i]);
    }
    free(compilation->headers);
    for (int i = 0; i < compilation->num_flags; i++)
    {
        free(compilation->flag_list[i]);
    }
    free(compilation->flag_list);
    for (int i = 0; i < compilation->num_libs; i++)
    {
        free(compilation->libraries[i]);
    }
    free(compilation->libraries);
    free(compilation);
}
