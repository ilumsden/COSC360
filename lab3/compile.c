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
    compilation->num_commands = 0;
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
            if (access(file, F_OK) < 0)
            {
                fprintf(stderr, "Error: %s does not exist.\n", file);
                exit(-1);
            }
            else if (file[strlen(file)-2] != '.' && file[strlen(file)-1] != 'c')
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
            if (access(file, F_OK) < 0)
            {
                fprintf(stderr, "Error: %s does not exist.\n", file);
                exit(-1);
            }
            else if (file[strlen(file)-2] != '.' && file[strlen(file)-1] != 'h')
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
            if (access(file, F_OK) < 0)
            {
                fprintf(stderr, "Error: %s does not exist.\n", file);
                exit(-1);
            }
            else if (strcmp(&file[strlen(file)-2], ".a") != 0 
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
    bool *recompile = (bool*) malloc(compilation->num_srcs*sizeof(bool));
    char **objs = (char**) malloc(compilation->num_srcs*sizeof(char*));
    int num_objs = 0;
    int *header_times = (int*) malloc(compilation->num_headers*sizeof(int));
    for (int i = 0; i < compilation->num_headers; i++)
    {
        struct stat buf;
        if ( stat(compilation->headers[i], &buf) < 0 )
        {
            fprintf(stderr, "Error: %s doesn't exist.\n", compilation->headers[i]);
            exit(-1);
        }
        header_times[i] = (int) buf.st_mtime;
    }
    for (int i = 0; i < compilation->num_srcs; i++)
    {
        recompile[i] = false;
        struct stat buf;
        char* obj_name = (char*) malloc(strlen(compilation->srcs[i]));
        obj_name[0] = 0;
        strncpy(obj_name, compilation->srcs[i], strlen(compilation->srcs[i])-1);
        obj_name[strlen(compilation->srcs[i])-1] = 'o';
        objs[i] = strdup(obj_name);
        if (access(obj_name, F_OK) < 0)
        {
            recompile[i] = true;
            num_objs++;
        }
        else
        {
            if ( stat(compilation->srcs[i], &buf) < 0 )
            {
                fprintf(stderr, "Error: could not stat %s\n", compilation->srcs[i]);
                exit(-1);
            }
            int src_time = (int) buf.st_mtime;
            if ( stat(obj_name, &buf) < 0 )
            {
                fprintf(stderr, "Error: could not stat %s\n", obj_name);
                exit(-1);
            }
            int obj_time = (int) buf.st_mtime;
            if (src_time > obj_time)
            {
                recompile[i] = true;
                num_objs++;
            }
            else
            {
                for (int j = 0; j < compilation->num_headers; j++)
                {
                    if (header_times[j] > obj_time)
                    {
                        recompile[i] = true;
                        num_objs++;
                        break;
                    }
                }
            }
        }
        free(obj_name);
    }
    free(header_times);
    char **compile_lines = (char**) malloc((num_objs+1) * sizeof(char*));
    compilation->num_commands = num_objs+1;
    for (int i = 0; i < num_objs+1; i++)
    {
        compile_lines[i] = (char*) malloc(MAX_COMPILE_COMMAND_SIZE);
        compile_lines[i][0] = 0;
    }
    int idx = 0;
    for (int i = 0; i < compilation->num_srcs; i++)
    {
        if (recompile[i])
        {
            if (idx == num_objs+1)
            {
                fprintf(stderr, "Internal Error: Trying to recompile too many files.\n");
                exit(-1);
            }
            strcpy(compile_lines[idx], "gcc ");
            strcat(compile_lines[idx], "-c ");
            for (int j = 0; j < compilation->num_flags; j++)
            {
                strcat(compile_lines[idx], compilation->flag_list[j]);
                strcat(compile_lines[idx], " ");
            }
            strcat(compile_lines[idx], compilation->srcs[i]);
            idx++;
        }
    }
    strcpy(compile_lines[idx], "gcc -o ");
    strcat(compile_lines[idx], compilation->exec_name);
    strcat(compile_lines[idx], " ");
    for (int i = 0; i < compilation->num_flags; i++)
    {
        strcat(compile_lines[idx], compilation->flag_list[i]);
        strcat(compile_lines[idx], " ");
    }
    for (int i = 0; i < compilation->num_srcs; i++)
    {
        if (recompile[i])
        {
            strcat(compile_lines[idx], objs[i]);
            strcat(compile_lines[idx], " ");
        }
    }
    for (int i = 0; i < compilation->num_libs; i++)
    {
        strcat(compile_lines[idx], compilation->libraries[i]);
        strcat(compile_lines[idx], " ");
    }
    free(recompile);
    for (int i = 0; i < compilation->num_srcs; i++)
    {
        free(objs[i]);
    }
    free(objs);
    return compile_lines;
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
