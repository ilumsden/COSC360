#include "ip_parser.h"

unsigned int intcat(unsigned int a, unsigned int b)
{
    unsigned int p = 10;
    // Sets p so that a will not overlap with b
    while (b >= p) p *= 10;
    // Calculates the concatenated integer and returns it
    return a * p + b;
}

IP* new_ip()
{
    // Allocates the new IP struct
    IP* ip = (IP*) memChk(malloc(sizeof(IP)));
    // Initalizes a JRB tree for the IP names
    ip->names = make_jrb();
    ip->num_names = 0;
    for (int i = 0; i < 4; i++)
    {
        ip->address_nums[i] = '\0';
    }
    // Allocates space for the IP address
    ip->address = (char*) memChk(malloc(16*sizeof(char)));
    ip->address[0] = 0;
    return ip;
}

void gen_address(IP *ip)
{
    // Converts the contents of address_nums into a IP address using sprintf
    int a0 = (int) ip->address_nums[0];
    int a1 = (int) ip->address_nums[1];
    int a2 = (int) ip->address_nums[2];
    int a3 = (int) ip->address_nums[3];
    sprintf(ip->address, "%d.%d.%d.%d", a0, a1, a2, a3);
}

void read_bin_data_fpointer(IP *ip, FILE *stream)
{
    // Reads the first four bytes of data as the numbers that make up the address
    fread(ip->address_nums, sizeof(unsigned char), 4, stream);
    // Creates the actual address string
    gen_address(ip);
    // Calculates the number of listed names from the next 4 bytes of data
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
        // Initialization of required data for obtaining name
        name = (char*) memChk(malloc(MAX_NAME_LENGTH));
        name[0] = 0;
        idx = 0;
        locallen = 0;
        absolute = false;
        // Constructs the current name character-by-character until it reaches a nul
        // terminator or EOF or until it reads in too much data for the name's memory.
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
            // If a name has a period in it, it is an absolute name.
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
        // Inserts the name into a JRB to alphabetize it
        jrb_insert_str(ip->names, name, new_jval_v(NULL));
        ip->num_names++;
        // If the name is absolute, its contents before the first period is copied
        // into a new string and added to the JRB.
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
            ip->num_names++;
        }
    }
    // This goto point is done to allow for non-fatal exitting.
    epoint:
        return;
}

void read_bin_data_sys(IP *ip, int stream)
{
    // Reads in the first four bytes as the IP address numbers.
    if ( read(stream, ip->address_nums, 4) < 0 )
    {
        fprintf(stderr, "Warning: EOF reached during read of address.\n");
        return;
    }
    // Creates the IP address string
    gen_address(ip);
    unsigned int numNames = 0;
    unsigned char ch = 0;
    // Gets the number of names from the next four bytes
    for (int i = 0; i < 4; i++)
    {
        if ( read(stream, &ch, sizeof(unsigned char)) < 0 )
        {
            fprintf(stderr, "Warning: EOF reached during read of number of names.\n");
            return;
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
        char c = 0;
        absolute = false;
        // Constructs the current name character-by-character until it reaches a nul
        // terminator or EOF or until it reads in too much data for the name's memory.
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
                fprintf(stderr, "Warning: EOF reached during character read\n");
                return;
            }
            name[idx] = c;
            // If a name has a period in it, it is an absolute name.
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
        // Inserts the name into a JRB to alphabetize it
        jrb_insert_str(ip->names, name, new_jval_v(NULL));
        ip->num_names++;
        // If the name is absolute, its contents before the first period is copied
        // into a new string and added to the JRB.
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
            ip->num_names++;
        }
    }
    // See read_bin_data_fpointer
    epoint:
        return;
}

void read_bin_data_buf(IP *ip, char *buf, int *current_loc)
{
    // Gets the IP address numbers from the buffer
    memcpy(ip->address_nums, buf+(*current_loc), 4);
    (*current_loc) += 4;
    // Generates the IP address string
    gen_address(ip);
    unsigned int numNames = 0;
    unsigned char ch = 0;
    // Gets the number of names from the next four bytes
    for (int i = 0; i < 4; i++)
    {
        memcpy(&ch, buf+(*current_loc), 1);
        (*current_loc)++;
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
        char c = 0;
        absolute = false;
        // Constructs the current name character-by-character until it reaches a nul
        // terminator or EOF or until it reads in too much data for the name's memory.
        while (1)
        {
            if (idx == MAX_NAME_LENGTH - 2)
            {
                name[idx] = '\0';
                printf("Could not read full name. Extracted name is %s\n", name);
                break;
            }
            memcpy(&c, buf+(*current_loc), 1);
            (*current_loc)++;
            name[idx] = c;
            // If a name has a period in it, it is an absolute name.
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
        // Inserts the name into a JRB to alphabetize it
        jrb_insert_str(ip->names, name, new_jval_v(NULL));
        ip->num_names++;
        // If the name is absolute, its contents before the first period is copied
        // into a new string and added to the JRB.
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
            ip->num_names++;
        }
    }
    // See read_bin_data_fpointer
    epoint:
        return;
}

void print_data(IP *ip, FILE *stream)
{
    // Prints the IP Adress
    fprintf(stream, "%s:  ", ip->address);
    JRB tmp;
    JRB nil = jrb_nil(ip->names);
    // Prints each name unless it cannot be accessed
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
