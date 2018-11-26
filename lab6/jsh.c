#include "piper.h"
#include <ctype.h>

// Macro that defines the maximum allowed size for a single line in the shell
#define MAX_COMMAND_LEN 2048

// Removes leading and trailing whitespace from the command
char* trim_ws(char* input)
{
    // Moves the pointer so that it points to the first non-whitespace character
    while (isspace((unsigned char) *input)) {++input;}
    // If the string was all whitespace, return an empty string.
    if (*input == 0)
    {
        return input;
    }
    // Moves the end pointer to the last non-whitespace character in the string.
    char *end = input + strlen(input) - 1;
    while (end > input && isspace((unsigned char) *end)) {--end;}
    // Sets the first trailing whitespace character to the NULL-character to terminate the string.
    end[1] = 0;
    // Returns the trimmed string
    return input;
}

// Splits the input string by spaces
char** split_command(char* input, int *num_spaces)
{
    // Gets the first word in the command
    char **args = NULL;
    char *p = strtok(input, " \n");
    // num_spaces (dereferenced) stores the number of terms in the command
    *num_spaces = 0;
    while (p)
    {
	// Reallocates the array of strings to fit the next element.
	// Then, *num_spaces is incremented, and the next term in appended to the array.
        args = (char**) realloc(args, ++(*num_spaces)*sizeof(char*));
        if (args == NULL)
        {
            fprintf(stderr, "Error: could not allocate memory for input splitting.\n");
            exit(-1);
        }
        args[*(num_spaces)-1] = p;
	// Gets the next term in the command
        p = strtok(NULL, " \n");
    }
    // Adds NULL to the end of the array
    args = (char**) realloc(args, ++(*num_spaces)*sizeof(char*));
    if (args == NULL)
    {
        fprintf(stderr, "Error: could not allocate memory for input splitting.\n");
        exit(-1);
    }
    args[(*num_spaces)-1] = NULL;
    return args;
}

int main(int argc, char **argv)
{
    // If a command line argument was provided, it is used as the prompt.
    // Otherwise, a default prompt is used.
    char *prompt;
    if (argc == 2)
    {
	// If the command line argument is a dash (-), no prompt is used.
        if (strcmp(argv[1], "-") == 0)
        {
            prompt = strdup("");
        }
        else
        {
            prompt = strdup(argv[1]);
        }
    }
    else
    {
        prompt = strdup("jsh:");
    }
    char buf[MAX_COMMAND_LEN];
    // If there is a prompt, display it.
    if (strcmp(prompt, "") != 0)
    {
        printf("%s ", prompt);
    }
    // Continue reading commands until stdin is closed.
    while (fgets(buf, MAX_COMMAND_LEN, stdin) != NULL)
    {
        // Trim whitespace from the input line.
	// If the input line is only whitespace, it is ignored, and the shell
	// will print the prompt and await user input.
        char *input = trim_ws(buf);
        if (strcmp(input, "") == 0)
        {
            if (strcmp(prompt, "") != 0)
            {
                printf("%s ", prompt);
            }
            continue;
        }
	// Splits the trimmed input line into an array of words ending with NULL.
        int num_coms = 0;
        char** command = split_command(input, &num_coms);
	// Offloads the remainder of the logic to a Piper object
        Piper p = create_piper(command, num_coms);
        int ret = run_commands(p);
        free_piper(p);
	// If run_commands returns 1, the user entered exit.
	// So, the shell shuts down.
	// Otherwise, the prompt is printed (if there is one), and the shell
	// awaits more user input.
        if (ret == 1)
        {
            break;
        }
        if (strcmp(prompt, "") != 0)
        {
            printf("%s ", prompt);
        }
    }
    free(prompt);
}
