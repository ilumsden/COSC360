#include "server_tasks.h"

Client new_client(char *join_message, time_t t)
{
    Client cli = (Client) malloc(sizeof(struct client_t));
    int ind = 0;
    for (int i = 0; i < (int) strlen(join_message); i++)
    {
        if (join_message[i] == ':')
        {
            ind = i;
            break;
        }
    }
    char *name = (char*) malloc(ind+2);
    strncpy(name, join_message, ind);
    cli->username = name;
    cli->connection_id = num_connections + 1;
    cli->creation_time = ctime(&t);
    cli->talk_time = ctime(&t);
    cli->quit_time = NULL;
    cli->stat = active;
    return cli;
}

void print_client(Client cli, bool print_creation)
{
    if (!print_creation)
    {
        printf("%-2d %12s last talked at %s", cli->connection_id, cli->username, cli->talk_time);
    }
    else
    {
        printf("%-2d %12s  %s\n", cli->connection_id, cli->username, cli->stat);
        printf("   Joined      at %s", cli->creation_time);
        printf("   Last talked at %s", cli->talk_time);
        if (cli->quit_time != NULL)
        {
            printf("   Quit        at %s", cli->quit_time);
        }
    }
}

void add_client(char *join_message, time_t t)
{
    Client cli = new_client(join_message, t);
    num_connections++;
    jrb_insert_int(current_clients, (int) cli->connection_id, new_jval_v((void*) cli));
    jrb_insert_int(all_clients, (int) cli->connection_id, new_jval_v((void*) cli));
}

void print_current_clients()
{
    JRB ptr;
    jrb_traverse(ptr, current_clients)
    {
        Client cli = (Client) ptr.val.v;
        print_client(cli, false);
    }
}

void print_all_clients()
{
    JRB ptr;
    jrb_traverse(ptr, all_clients)
    {
        Client cli = (Client) ptr.val.v;
        print_client(cli, true);
    }
}

void jtalk_console()
{
    char command[10];
    printf("Jtalk_server_console> ");
    while (read(stdin, command, 10) != -1)
    {
        if (strcmp(command, "TALKERS") == 0)
        {
            print_current_clients();
        }
        else if (strcmp(command, "ALL") == 0)
        {
            print_all_clients();
        }
        else
        {
            printf("Unknown console command: %s\n", command);
        }
    }
    fflush(stdout);
    pthread_exit();
}
