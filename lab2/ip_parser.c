#include "ip_parser.h"

unsigned int intcat(unsigned int a, unsigned int b)
{
    unsigned int p = 10;
    while (b >= p) p *= 10;
    return a * p + b;
}

IP* new_ip()
{
    IP* ip = (IP*) memChk(malloc(sizeof(IP)));
    ip->names = new_dllist();
    ip->address_nums = (unsigned char*) memChk(malloc(4*sizeof(unsigned char)));
    for (int i = 0; i < 4; i++)
    {
        ip->address_nums[i] = '\0';
    }
    ip->address = (char*) memChk(malloc(16*sizeof(char)));
    ip->address[0] = 0;
    return ip;
}

void gen_address(IP *ip)
{
     int a0 = (int) ip->address_nums[0];
     int a1 = (int) ip->address_nums[1];
     int a2 = (int) ip->address_nums[2];
     int a3 = (int) ip->address_nums[3];
     //char *addr = (char*) memChk(malloc(15));
     //addr[0] = 0;
     sprintf(ip->address, "%d.%d.%d.%d", a0, a1, a2, a3);
     //printf("addr is %s\n", ip->address);
     //return addr;
}

void read_bin_data(IP *ip, FILE *stream)
{
    fread(ip->address_nums, sizeof(unsigned char), 4, stream);
    gen_address(ip);
    unsigned int numNames = 0;
    unsigned char ch = 0;
    for (int i = 0; i < 4; i++)
    {
        fread(&ch, sizeof(unsigned char), 1, stream);
        numNames = intcat(numNames, (unsigned int) ch);
    }
    /*char num[13];
    num[0] = 0;
    unsigned char comp;
    char str_comp[4];
    str_comp[0] = 0;
    int int_comp;
    for (int i = 0; i < 4; i++)
    {
        fread(&comp, sizeof(unsigned char), 1, stream);
        int_comp = (int) comp;
        sprintf(str_comp, "%d", int_comp);
        strcpy(num+strlen(num), str_comp);
        //strcat(num, str_comp);
    }
    char *endptr;
    numNames = (int) strtoimax(num, &endptr, 10);*/
    //printf("numNames is %d\n", numNames);
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
            idx++;
        }
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
        return;
    }
}

void print_data(IP *ip, FILE *stream)
{
    fprintf(stream, "%s: ", ip->address);
    /*for (int i = 0; i < 4; i++)
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
    }*/
    Dllist tmp;
    Dllist nil = dll_nil(ip->names);
    dll_traverse(tmp, ip->names)
    {
        if (tmp == nil)
        {
            continue;
        }
        char *name = (char*) tmp->val.s;
        fprintf(stream, "name is %s\n", name);
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
    Dllist tmp;
    Dllist nil = dll_nil(ip->names);
    dll_traverse(tmp, ip->names)
    {
        if (tmp == nil)
        {
            continue;
        }
        free(tmp->val.s);
    }
    free_dllist(ip->names);
    free(ip->address);
    free(ip->address_nums);
    free(ip);
}
