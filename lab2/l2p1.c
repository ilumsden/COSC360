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
    Dllist ip_list = new_dllist();
    Dllist dtmp;
    Dllist dnil;
    while (!feof(stream))
    {
        ip = new_ip();
        read_bin_data(ip, stream);
        dnil = dll_nil(ip->names);
        dll_traverse(dtmp, ip->names)
        {
            if (dtmp == dnil)
            {
                continue;
            }
            char *name = dtmp->val.s;
            if (name == NULL || strcmp(name, "") == 0)
            {
                perror("Error: name is empty before insert");
                return -1;
            }
            jrb_insert_str(ip_tree, name, new_jval_v((void*)ip));
        }
        dll_append(ip_list, new_jval_v((void*)ip));
    }
    fclose(stream);
    JRB tmp;
    JRB nil = jrb_nil(ip_tree);
    jrb_traverse(tmp, ip_tree)
    {
        if (tmp == nil)
        {
            continue;
        }
        IP *cur = (IP*) tmp->val.v;
        if (cur == NULL)
        {
            perror("Internal Error: tree node does not contain an IP object.");
            return -1;
        }
        //print_data(cur, stdout);
        //printf("\n");
    }
    dnil = dll_nil(ip_list);
    dll_traverse(dtmp, ip_list)
    {
        if (dtmp == dnil)
        {
            continue;
        }
        ip = (IP*) dtmp->val.v;
        free_ip(ip);
    }
    jrb_free_tree(ip_tree);
    free_dllist(ip_list);
}
