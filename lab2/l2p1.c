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
        char *address = get_address(ip);
        jrb_insert_str(ip_tree, address, new_jval_v((void*)ip));
    }
    JRB tmp;
    jrb_traverse(tmp, ip_tree)
    {
        IP *cur = (IP*) tmp->val.v;
        print_data(cur, stdout);
    }
}
