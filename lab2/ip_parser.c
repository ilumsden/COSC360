#include "ip_parser.h"

IP* new_ip()
{
    IP* ip = (IP*) memChk(malloc(sizeof(IP)));
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
    int numNames = 0;
    char num[12];
    num[0] = 0;
    unsigned char comp;
    char str_comp[3];
    str_comp[0] = 0;
    int int_comp;
    for (int i = 0; i < 4; i++)
    {
        fread(&comp, sizeof(unsigned char), 1, stream);
        int_comp = (int) comp;
        sprintf(str_comp, "%d", int_comp);
        strcat(num, str_comp);
    }
    char *endptr;
    numNames = (int) strtoimax(num, &endptr, 10);
    char *name;
    int idx;
    bool absolute;
    for (int i = 0; i < numNames; i++)
    {
        name = (char*) memChk(malloc(MAX_NAME_LENGTH));
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
        printf("name is %s\n", name);
        dll_append(ip->names, new_jval_s(name));
        if (absolute)
        {
            char *end_ptr = strchr(name, '.');
            if (end_ptr == NULL)
            {
                fprintf(stderr, "Internal Error: Name (%s) is supposedly absolute, but could not find dot", name);
                goto epoint;
            }
            int len = end_ptr - name;
            char *local = (char*) memChk(malloc(len));
            for (int i = 0; i < len; i++)
            {
                local[i] = name[i];
            }
            dll_append(ip->names, new_jval_s(local));
        }
    epoint:
        exit(1);
    }
}

char* get_address(IP *ip)
{
     int a0 = (int) ip->address[0];
     int a1 = (int) ip->address[1];
     int a2 = (int) ip->address[2];
     int a3 = (int) ip->address[3];
     char *addr = (char*) memChk(malloc(15));
     sprintf(addr, "%d.%d.%d.%d", a0, a1, a2, a3);
     return addr;
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
