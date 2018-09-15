#include "ip_parser.h"

#include <stdbool.h>

IP* new_ip()
{
    IP* ip = (IP*) malloc(sizeof(IP));
    ip->names = new_dllist();
    for (int i = 0; i < 4; i++)
    {
        ip->address[i] = '\0';
    }
    return ip;
}

void read_bin_data(IP *ip, FILE *stream)
{
    fread(ip->names, 1, 4, stream);
    int numNames;
    fread(&numNames, sizeof(int), 1, stream);
    char *name;
    int idx;
    bool absolute;
    for (int i = 0; i < numNames; i++)
    {
        name = (char*) malloc(MAX_NAME_LENGTH);
        idx = 0;
        absolute = false;
        while (1)
        {
            if (idx == MAX_NAME_LENGTH - 2)
            {
                name[idx] = '\0';
                printf("Could not read full name. Extracted name is %s\n", name);
                break;
            }
            char c = fgetc(stream);
            name[idx] = c;
            if (c == '.' && !absolute)
            {
                absolute = true;
            }
            if (c == '\0')
            {
                break;
            }
        }
        dll_append(ip->names, new_jval_s(name));
        if (absolute)
        {
            char *end_ptr = strchr(name, '.');
            int len = end_ptr - name;
            char *local = (char*) malloc(len);
            for (int i = 0; i < len; i++)
            {
                local[i] = name[i];
            }
            dll_append(ip->names, new_jval_s(local));
        }
    }
}

void print_data(IP *ip, FILE *stream)
{
    for (int i = 0; i < 4; i++)
    {
        fprintf(stream, "%d", (int)ip->address[i]);
        if (i != 3)
        {
            fprintf(stream, ".");
        }
        else
        {
            fprintf(stream, ": ");
        }
    }
    Dllist tmp;
    dll_traverse(tmp, ip->names)
    {
        char *name = (char*) tmp->val.s;
        if (name == NULL || strcmp(name, "") == 0)
        {
            perror("Error: invalid name for this IP.");
            exit(-1);
        }
        fprintf(stream, "%s ", name);
    }
}

void free_ip(IP *ip)
{
    free_dllist(ip->names);
    free(ip);
}
