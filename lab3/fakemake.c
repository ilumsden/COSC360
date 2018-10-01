#include "compile.h"
#include "fields.h"

/* fakemake.c
 * Author: Ian Lumsden
 *
 * Reads in a template file, and uses the data to compile the desired executable.
 */

// Extracts the source file from an object compilation command
char* recover_fname_from_command(char *command)
{
    int fnamelen = 0;
    int startidx = -1;
    // Gets the starting index and length of the source file.
    for (int i = strlen(command)-1; i >= 0; i--)
    {
        if (command[i] == ' ')
        {
            startidx = i+1;
            fnamelen = strlen(command) - startidx;
            break;
        }
    }
    // This means that there was not a space in the command.
    if (startidx == -1)
    {
        return command;
    }
    // Allocates and copies the file name
    char *fname = (char*) malloc(fnamelen+1);
    strcpy(fname, &command[startidx]);
    return fname;
}

int main(int argc, char **argv)
{
    // The default target is fmakefile. It can be changed by passing it as the first
    // user-provided command-line argument
    char target[50] = "fmakefile";
    if (argc == 2)
    {
        strcpy(target, argv[1]);
    }
    IS is = new_inputstruct(target);
    Compile *compilation = new_compilation();
    while (get_line(is) >= 0)
    {
        // Do nothing if an empty line is read
        if (is->NF == 0)
        {
            continue;
        }
        // Call add_exec if an E line is read
        else if (strcmp(is->fields[0], "E") == 0)
        {
            add_exec(compilation, is->fields, is->NF, is->line);
        }
        // Call add_srcs if a S line is read
        else if (strcmp(is->fields[0], "C") == 0)
        {
            add_srcs(compilation, is->fields, is->NF);
        }
        // Call add_headers if a H line is read
        else if (strcmp(is->fields[0], "H") == 0)
        {
            add_headers(compilation, is->fields, is->NF);
        }
        // Call add_flags if a F line is read
        else if (strcmp(is->fields[0], "F") == 0)
        {
            add_flags(compilation, is->fields, is->NF);
        }
        // Call add_libraries if a L line is read
        else if (strcmp(is->fields[0], "L") == 0)
        {
            add_libraries(compilation, is->fields, is->NF);
        }
        // Raise an error if an invalid line is read.
        else
        {
            fprintf(stderr, "Error: invalid line (%d)\n", is->line);
            return -1;
        }
    }
    jettison_inputstruct(is);
    // If no executable was specified, raise an error and exit.
    if (compilation->exec_name == NULL || strcmp(compilation->exec_name, "") == 0)
    {
        fprintf(stderr, "No executable specified\n");
        free_compilation(compilation);
        return -1;
    }
    char **commands = get_commands(compilation);
    for (int i = 0; i < compilation->num_commands-1; i++)
    {
        char *fname = recover_fname_from_command(commands[i]);
        // If the source file for the current command does not exist,
        // raise an error and exit.
        if ( access(fname, F_OK) != 0 )
        {
            fprintf(stderr, "fmakefile: %s: No such file or directory\n", fname);
            free(fname);
            for (int i = 0; i < compilation->num_commands; i++)
            {
                free(commands[i]);
            }
            free(commands);
            free_compilation(compilation);
            return -1;
        }
        free(fname);
        printf("%s\n", commands[i]);
        // If the compilation failed, raise an error and exit.
        if ( system(commands[i]) != 0 )
        {
            fprintf(stderr, "Command failed.  Exiting\n");
            for (int i = 0; i < compilation->num_commands; i++)
            {
                free(commands[i]);
            }
            free(commands);
            free_compilation(compilation);
            return -1;
        }
    }
    printf("%s\n", commands[compilation->num_commands-1]);
    // If the linking command failed, raise an error and exit.
    if ( system(commands[compilation->num_commands-1]) != 0 )
    {
        fprintf(stderr, "Command failed.  Fakemake exiting\n");
        return -1;
    }
    // Free remaining memory
    for (int i = 0; i < compilation->num_commands; i++)
    {
        free(commands[i]);
    }
    free(commands);
    free_compilation(compilation);
    return 0;
}
