#include "ip_parser.h"
#include "jrb.h"

int name_list_cmp(Jval list1, Jval list2)
{
    Dllist l1 = (Dllist) list1.v;
    Dllist l2 = (Dllist) list2.v;
    if (l1 == l2)
    {
        return 0;
    }
    else if (l1 == NULL && l2 != NULL)
    {
        return -1;
    }
    else if (l1 != NULL && l2 == NULL)
    {
        return 1;
    }
    else
    {
        char *n1 = (char*) dll_first(l1)->val.s;
        char *n2 = (char*) dll_first(l2)->val.s;
        return strcmp(n1, n2);
    }
}

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
        jrb_insert_gen(ip_tree, new_jval_v((void*)ip->names), new_jval_v((void*)ip), name_list_cmp);
    }
    JRB tmp;
    jrb_traverse(tmp, ip_tree)
    {
        IP *cur = (IP*) tmp->val.v;
        print_data(cur, stdout);
    }
}
