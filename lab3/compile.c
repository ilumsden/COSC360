#include "compile.h"

/* compile.c
 * Author: Ian Lumsden
 *
 * Contains the implementation of the functions declared in compile.h
 */

Compile* new_compilation()
{
    // Allocate the Compile object
    Compile *compilation = (Compile*) malloc(sizeof(Compile));
    // Allocate each string/string array.
    // For strings, initialize them to be empty and null terminated
    // For arrays, set the corresponding size number to 0
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
    // Set the number of commands to 0
    compilation->num_commands = 0;
    return compilation;
}

void add_srcs(Compile *compilation, char **fields, int NF)
{
    // Print a warning message if an empty line was interpreted as a C line
    if (NF == 0)
    {
        fprintf(stderr, "Warning: Read in an empty line as a sources line.\n");
    }
    // Print a warning message if a non-C line was interpreted as a C line
    else if (strcmp(fields[0], "C") != 0)
    {
        fprintf(stderr, "Warning: Read in a non-source line as a source line.\n");
    }
    else
    {
        for (int i = 1; i < NF; i++)
        {
            char *file = fields[i];
            // If the current file is not a .c, print a warning, and don't add it to the src list.
            if (file[strlen(file)-2] != '.' && file[strlen(file)-1] != 'c')
            {
                fprintf(stderr, "Warning: Non-source file included on a sources line. Skipping.\n");
            }
            // Otherwise, add the file (string copy) to the src list.
            else
            {
                compilation->srcs[compilation->num_srcs] = strdup(file);
                compilation->num_srcs++;
                // If the max number of sources have been reached, print a warning, and end the function.
                if (compilation->num_srcs == MAX_ARGS)
                {
                    fprintf(stderr, "Warning: Maximum number of sources reached (%d)\n", MAX_ARGS);
                    break;
                }
            }
        }
    }
}

void add_exec(Compile *compilation, char **fields, int NF, int linenum)
{
    // If a non-E line was read as an E line, print a warning.
    if (strcmp(fields[0], "E") != 0)
    {
        fprintf(stderr, "Warning: Read in a non-executable line as an executable line.\n");
    }
    // If an empty line was read as an E line, print a warning.
    else if (NF == 0)
    {
        fprintf(stderr, "Warning: Read in an empty line as an executable line.\n");
    }
    // If an E line has already been read, print an error message, and exit the program.
    else if (strcmp(compilation->exec_name, "") != 0)
    {
        fprintf(stderr, "fmakefile (%d) cannot have more than one E line\n", linenum);
        exit(-1);
    }
    // Otherwise, copy the executable name into the Compile object.
    else
    {
        strcpy(compilation->exec_name, fields[1]);
    }
}

void add_headers(Compile *compilation, char **fields, int NF)
{
    // Print a warning message if an empty line was interpreted as a H line
    if (NF == 0)
    {
        fprintf(stderr, "Warning: Read in an empty line as a header line.\n");
    }
    // Print a warning message if a non-H line was interpreted as a H line
    else if (strcmp(fields[0], "H") != 0)
    {
        fprintf(stderr, "Warning: Read in a non-header line as a header line.\n");
    }
    else
    {
        for (int i = 1; i < NF; i++)
        {
            char *file = fields[i];
            // If the current file is not a .h, print a warning, and don't add it to the header list.
            if (file[strlen(file)-2] != '.' && file[strlen(file)-1] != 'h')
            {
                fprintf(stderr, "Warning: Non-header file included on a header line. Skipping.\n");
            }
            // Otherwise, add the file (string copy) to the header list.
            else
            {
                compilation->headers[compilation->num_headers] = strdup(file);
                compilation->num_headers++;
                // If the max number of headers have been reached, print a warning, and end the function.
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
    // Print a warning message if an empty line was interpreted as a F line
    if (NF == 0)
    {
        fprintf(stderr, "Warning: Read in an empty line as a flag line.\n");
    }
    // Print a warning message if a non-F line was interpreted as a F line
    else if (strcmp(fields[0], "F") != 0)
    {
        fprintf(stderr, "Warning: Read in a non-flag line as a flag line.\n");
    }
    else
    {
        for (int i = 1; i < NF; i++)
        {
            char *file = fields[i];
            // If the current file is not a flag (doesn't start with a "-"), 
            // print a warning, and don't add it to the flag list.
            if (file[0] != '-')
            {
                fprintf(stderr, "Warning: Non-flag included on a flag line. Skipping.\n");
            }
            // Otherwise, add the flag (string copy) to the flag list.
            else
            {
                compilation->flag_list[compilation->num_flags] = strdup(file);
                compilation->num_flags++;
                // If the max number of flags have been reached, print a warning, and end the function.
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
    // Print a warning message if an empty line was interpreted as a L line
    if (NF == 0)
    {
        fprintf(stderr, "Warning: Read in an empty line as a library line.\n");
    }
    // Print a warning message if a non-L line was interpreted as a L line
    else if (strcmp(fields[0], "L") != 0)
    {
        fprintf(stderr, "Warning: Read in a non-library line as a library line.\n");
    }
    else
    {
        for (int i = 1; i < NF; i++)
        {
            char *file = fields[i];
            // If the string represents a linking flag, add it to the library list.
            if (file[0] == '-' && (file[1] == 'l' || file[1] == 'L'))
            {
                compilation->libraries[compilation->num_libs] = strdup(file);
                compilation->num_libs++;
                // If the max number of libraries/links have been reached, print a warning, and end the function.
                if (compilation->num_libs == MAX_ARGS)
                {
                    fprintf(stderr, "Warning: Maximum number of libraries reached (%d)\n", MAX_ARGS);
                    break;
                }
            }
            // If the string represents a file, check if it exists.
            // If not, raise an error, and exit the program.
            else if (access(file, F_OK) < 0)
            {
                fprintf(stderr, "Error: %s does not exist.\n", file);
                exit(-1);
            }
            // If the library is not a .a (static library) or .so (shared library), prints a warning,
            // and doesn't add the library to the library list.
            else if ((file[strlen(file)-2] != '.' && file[strlen(file)-1] != 'a') &&
                     (file[strlen(file)-3] != '.' && file[strlen(file)-2] != 's' &&
                      file[strlen(file)-1] != 'o'))
            {
                fprintf(stderr, "Warning: Non-library included on a library line. Skipping.\n");
            }
            // Otherwise, add the library to the library list.
            else
            {
                compilation->libraries[compilation->num_libs] = strdup(file);
                compilation->num_libs++;
                // If the max number of libraries/links have been reached, print a warning, and end the function.
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
    // Allocates arrays for intermediate data
    bool *recompile = (bool*) malloc(compilation->num_srcs*sizeof(bool));
    char **objs = (char**) malloc(compilation->num_srcs*sizeof(char*));
    int num_objs = 0;
    int *header_times = (int*) malloc(compilation->num_headers*sizeof(int));
    // Stores the modification times for the headers in header_times.
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
    int *obj_times = (int*) malloc(compilation->num_srcs*sizeof(int));
    for (int i = 0; i < compilation->num_srcs; i++)
    {
        recompile[i] = false;
        struct stat buf;
        // Extract the object file names from the source files.
        char* obj_name = (char*) malloc(strlen(compilation->srcs[i])+1);
        obj_name[0] = 0;
        strcpy(obj_name, compilation->srcs[i]);
        obj_name[strlen(compilation->srcs[i])-1] = 'o';
        objs[i] = strdup(obj_name);
        // If the object file doesn't exist, record that the object file needs to be
        // compiled, and increment the number of objects to be compiled.
        if (access(obj_name, F_OK) < 0)
        {
            recompile[i] = true;
            num_objs++;
        }
        else
        {
            // Get the source file modification time.
            if ( stat(compilation->srcs[i], &buf) < 0 )
            {
                fprintf(stderr, "Error: could not stat %s\n", compilation->srcs[i]);
                exit(-1);
            }
            int src_time = (int) buf.st_mtime;
            // Get the object file modification time
            if ( stat(obj_name, &buf) < 0 )
            {
                fprintf(stderr, "Error: could not stat %s\n", obj_name);
                exit(-1);
            }
            obj_times[i] = (int) buf.st_mtime;
            // If the source file has been modified more recently than the object file,
            // record that the object file needs to be recompiled, and increment the number
            // of objects to be compiled.
            if (src_time > obj_times[i])
            {
                recompile[i] = true;
                num_objs++;
            }
            else
            {
                // If any of the headers have been modified more recently than the object file,
                // record that the object file needs to be recompiled, and increment the number
                // of objects to be compiled.
                for (int j = 0; j < compilation->num_headers; j++)
                {
                    if (header_times[j] > obj_times[i])
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
    // Checks if there are no object files that need to be compiled and if the final
    // executable exists
    if (num_objs == 0 && access(compilation->exec_name, F_OK) == 0)
    {
        struct stat buf;
        if ( stat(compilation->exec_name, &buf) < 0 )
        {
            fprintf(stderr, "Error: could not stat %s\n", compilation->exec_name);
            exit(-1);
        }
        int exec_time = (int) buf.st_mtime;
        bool up_to_date = true;
        // Sets up_to_date to false iff any of the object files have been modified more
        // recently than the executable.
        for (int i = 0; i < compilation->num_srcs; i++)
        {
            if (obj_times[i] > exec_time)
            {
                up_to_date = false;
                break;
            }
        }
        // Frees memory and exits the program if nothing needs to be complied.
        if (up_to_date)
        {
            printf("%s up to date\n", compilation->exec_name);
            free(recompile);
            for (int i = 0; i < compilation->num_srcs; i++)
            {
                free(objs[i]);
            }
            free(objs);
            free(obj_times);
            free_compilation(compilation);
            exit(1);
        }
    }
    char **compile_lines = (char**) malloc((num_objs+1) * sizeof(char*));
    compilation->num_commands = num_objs+1;
    // Initializes all the command strings
    for (int i = 0; i < num_objs+1; i++)
    {
        compile_lines[i] = (char*) malloc(MAX_COMPILE_COMMAND_SIZE);
        compile_lines[i][0] = 0;
    }
    int idx = 0;
    for (int i = 0; i < compilation->num_srcs; i++)
    {
        // Only do something if the object file needs to be compiled
        if (recompile[i])
        {
            // Internal error check to see if too many object files are trying to be recompiled.
            if (idx == num_objs+1)
            {
                fprintf(stderr, "Internal Error: Trying to recompile too many files.\n");
                exit(-1);
            }
            // Puts together the compilation command for the object file.
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
    // Puts together the compilation command (linking) for the final executable.
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
        strcat(compile_lines[idx], objs[i]);
        // Makes sure there isn't an extra space at the end of a command without libraries.
        if (i != compilation->num_srcs - 1)
        {
            strcat(compile_lines[idx], " ");
        }
    }
    for (int i = 0; i < compilation->num_libs; i++)
    {
        strcat(compile_lines[idx], " ");
        strcat(compile_lines[idx], compilation->libraries[i]);
    }
    // Frees temporary memory
    free(recompile);
    for (int i = 0; i < compilation->num_srcs; i++)
    {
        free(objs[i]);
    }
    free(objs);
    free(obj_times);
    return compile_lines;
}

void free_compilation(Compile *compilation)
{
    // Frees all the Compile object's memory
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
