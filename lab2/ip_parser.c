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
    ip->names = make_jrb();
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
     sprintf(ip->address, "%d.%d.%d.%d", a0, a1, a2, a3);
}

void read_bin_data_fpointer(IP *ip, FILE *stream)
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
    char *name;
    int idx;
    int locallen;
    bool absolute;
    for (int i = 0; i < numNames; i++)
    {
        name = (char*) memChk(malloc(MAX_NAME_LENGTH));
        name[0] = 0;
        idx = 0;
        locallen = 0;
        absolute = false;
        while (1)
        {
            if (idx == MAX_NAME_LENGTH - 2)
            {
                name[idx] = '\0';
                printf("Could not read full name. Extracted name is %s\n", name);
                break;
            }
            char c = (char) fgetc(stream);
            if (feof(stream))
            {
                perror("Error: the file ended in the middle of a name read");
                exit(-1);
            }
            name[idx] = c;
            if (c == '.' && !absolute)
            {
                locallen = idx+1;
                absolute = true;
            }
            if (c == '\0')
            {
                break;
            }
            ++idx;
        }
        jrb_insert_str(ip->names, name, new_jval_v(NULL));
        if (absolute)
        {
            if (locallen == 0)
            {
                fprintf(stderr, "Internal Error: Name (%s) is supposedly absolute, but could not find dot", name);
                goto epoint;
            }
            char *local = (char*) memChk(malloc(locallen));
            local[0] = 0;
            strncpy(local, name, locallen-1);
            local[locallen-1] = 0;
            jrb_insert_str(ip->names, local, new_jval_v(NULL));
        }
    }
    epoint:
        return;
}

int read_bin_data_sys(IP *ip, int stream)
{
    if ( read(stream, ip->address_nums, 4*sizeof(unsigned char)) < 0 )
    {
        return 1;
    }
    gen_address(ip);
    unsigned int numNames = 0;
    unsigned char ch = 0;
    for (int i = 0; i < 4; i++)
    {
        if ( read(stream, &ch, sizeof(unsigned char)) < 0 )
        {
            return 1;
        }
        numNames = intcat(numNames, (unsigned int) ch);
    }
    char *name;
    int idx;
    int locallen;
    bool absolute;
    for (int i = 0; i < numNames; i++)
    {
        name = (char*) memChk(malloc(MAX_NAME_LENGTH));
        name[0] = 0;
        idx = 0;
        locallen = 0;
        absolute = false;
        while (1)
        {
            if (idx == MAX_NAME_LENGTH - 2)
            {
                name[idx] = '\0';
                printf("Could not read full name. Extracted name is %s\n", name);
                break;
            }
            if ( read(stream, &c, sizeof(char)) < 0 )
            {
                return 1;
            }
            name[idx] = c;
            if (c == '.' && !absolute)
            {
                locallen = idx+1;
                absolute = true;
            }
            if (c == '\0')
            {
                break;
            }
            ++idx;
        }
        jrb_insert_str(ip->names, name, new_jval_v(NULL));
        if (absolute)
        {
            if (locallen == 0)
            {
                fprintf(stderr, "Internal Error: Name (%s) is supposedly absolute, but could not find dot", name);
                goto epoint;
            }
            char *local = (char*) memChk(malloc(locallen));
            local[0] = 0;
            strncpy(local, name, locallen-1);
            local[locallen-1] = 0;
            jrb_insert_str(ip->names, local, new_jval_v(NULL));
        }
    }
    epoint:
        return;
}

void print_data(IP *ip, FILE *stream)
{
    fprintf(stream, "%s: ", ip->address);
    JRB tmp;
    JRB nil = jrb_nil(ip->names);
    jrb_traverse(tmp, ip->names)
    {
        if (tmp == nil)
        {
            continue;
        }
        char *name = (char*) tmp->key.s;
        if (name == NULL || strcmp(name, "") == 0)
        {
            perror("Error: empty name.");
            exit(-1);
        }
        fprintf(stream, "%s ", name);
    }
    fprintf(stream, "\n");
}

void free_ip(IP *ip)
{
    JRB tmp;
    JRB nil = jrb_nil(ip->names);
    jrb_traverse(tmp, ip->names)
    {
        if (tmp == nil)
        {
            continue;
        }
        char *name = tmp->key.s;
        free(name);
    }
    jrb_free_tree(ip->names);
    free(ip->address);
    free(ip);
}
