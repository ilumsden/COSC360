#include "ip_parser.h"

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
    JRB alphabetizer = make_jrb();
    Dllist ip_list = new_dllist();
    JRB tmp;
    JRB nil;
    while (!feof(stream))
    {
        ip = new_ip();
        read_bin_data(ip, stream);
        nil = jrb_nil(ip->names);
        jrb_traverse(tmp, ip->names)
        {
            if (tmp == nil)
            {
                continue;
            }
            char *name = tmp->key.s;
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
    /*nil = jrb_nil(ip_tree);
    jrb_traverse(tmp, ip_tree)
    {
        if (tmp == nil)
        {
            printf("Failure\n");
            return -1;
        }
        IP *itmp = (IP*) tmp->val.v;
        print_data(itmp, stdout);
    }*/
    printf("Hosts all read in\n\n");
    char input[MAX_NAME_LENGTH];
    input[0] = 0;
    printf("Enter host name: ");
    JRB searchNode;
    while (!feof(stdin))
    {
        scanf("%s", input);
        if (feof(stdin))
        {
            fprintf(stderr, "Error: stdin closed in the middle of a read.\n");
            return -1;
        }
        if (ferror(stdin))
        {
            fprintf(stderr, "Error: could not successfully read user input\n");
        }
        searchNode = jrb_find_str(ip_tree, input);
        if (searchNode == NULL)
        {
            printf("no key %s\n", input);
        }
        else
        {
            ip = (IP*) searchNode->val.v;
            jrb_insert_str(alphabetizer, dll_first(ip->names)->val.s, new_jval_v((void*)ip));
            //print_data(ip, stdout);
            //printf("\n");
            while (1)
            {
                searchNode = jrb_prev(searchNode);
                //printf("key is %s\n", searchNode->key.s);
                if (strcmp(searchNode->key.s, input) != 0)
                {
                    break;
                }
                ip = (IP*) searchNode->val.v;
                jrb_insert_str(alphabetizer, dll_first(ip->names)->val.s, new_jval_v((void*)ip));
                //print_data(ip, stdout);
                //printf("\n");
            }
        }
        nil = jrb_nil(alphabetizer);
        jrb_traverse(tmp, alphabetizer)
        {
            if (tmp == nil)
            {
                continue;
            }
            ip = (IP*) tmp->val.v;
            print_data(ip, stdout);
            printf("\n");
        }
        jrb_free_tree(alphabetizer);
        alphabetizer = make_jrb();
        printf("Enter host name: ");
    }
    //print_data(cur, stdout);
    //printf("\n");
    Dllist dtmp;
    Dllist dnil = dll_nil(ip_list);
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
