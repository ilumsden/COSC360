#include "ip_parser.h"
#include "jrb.h"

int main(int argc, char **argv)
{
    FILE *stream = fopen("converted", "r");
    if (stream == NULL)
    {
        perror("Error: could not open converted.");
        return -1;
    }
    IP *ip;
    JRB ip_tree = make_jrb();
    while (!feof(stream))
    {
        ip = new_ip();
        read_bin_data(ip, stream);
        printf("address is %s\n", ip->address);
        jrb_insert_str(ip_tree, ip->address, new_jval_v((void*)ip));
    }
    fclose(stream);
    JRB tmp;
    jrb_traverse(tmp, ip_tree)
    {
        IP *cur = (IP*) tmp->val.v;
        print_data(cur, stdout);
        printf("\n");
    }
    jrb_traverse(tmp, ip_tree)
    {
        char *addr = tmp->key.s;
        free(addr);
        ip = (IP*) tmp->val.v;
        free_ip(ip);
    }
    jrb_free_tree(ip_tree);
}
