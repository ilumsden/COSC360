#include "server_tasks.h"

void send_bytes(char *p, int len, int fd)
{
    char *ptr;
    int i;

    ptr = p;
    while(ptr < p+len) 
    {
        i = write(fd, ptr, p-ptr+len);
        if (i < 0) 
        {
            perror("write");
            exit(1);
        }
        ptr += i;
    }
}

void send_string(char *s, int fd)
{
    int len;

    len = strlen(s);
    send_bytes((char *) &len, sizeof(int), fd);
    send_bytes(s, len, fd);
}

void shutdown(int sock)
{
    pthread_mutex_lock(mut);
    for (int i = 0; i < num_connections + 1; i++)
    {
        if (pthread_cancel(threads[i]) != 0)
        {
            perror("Could not cancel thread. Resorting to exit.");
        }
    }
    JRB ptr;
    jrb_traverse(ptr, current_clients_by_fd)
    {
        Client cli = (Client) ptr->val.v;
        close(cli->fd);
        free_client(cli);
    }
    jrb_free_tree(current_clients);
    jrb_free_tree(all_clients);
    jrb_free_tree(current_clients_by_fd);
    pthread_mutex_unlock(mut);
    if (pthread_mutex_destroy(mut) != 0)
    {
        perror("Could not destroy mutex");
    }
    close(sock);
    exit(0);
}

Client new_client(char *join_message, time_t t, int fd)
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
    name[0] = 0;
    strncpy(name, join_message, ind);
    cli->username = name;
    cli->connection_id = num_connections + 1;
    cli->creation_time = ctime(&t);
    cli->talk_time = ctime(&t);
    cli->quit_time = NULL;
    cli->stat = active;
    cli->fd = fd;
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

void add_client(char *join_message, time_t t, int fd)
{
    Client cli = new_client(join_message, t, fd);
    pthread_mutex_lock(mut);
    num_connections++;
    jrb_insert_int(current_clients, (int) cli->connection_id, new_jval_v((void*) cli));
    jrb_insert_int(all_clients, (int) cli->connection_id, new_jval_v((void*) cli));
    jrb_insert_int(current_clients_by_fd, cli->fd, new_jval_v((void*) cli));
    pthread_mutex_unlock(mut);
}

void free_client(Client cli)
{
    free(cli->username);
    free(cli);
}

void print_current_clients()
{
    JRB ptr;
    pthread_mutex_lock(mut);
    jrb_traverse(ptr, current_clients)
    {
        Client cli = (Client) ptr.val.v;
        print_client(cli, false);
    }
    pthread_mutex_unlock(mut);
}

void print_all_clients()
{
    JRB ptr;
    pthread_mutex_lock(mut);
    jrb_traverse(ptr, all_clients)
    {
        Client cli = (Client) ptr.val.v;
        print_client(cli, true);
    }
    pthread_mutex_unlock(mut);
}

void jtalk_console()
{
    char command[10];
    printf("Jtalk_server_console> ");
    while (read(stdin, command, 10) > 0)
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
    return;
}

void send_message_to_clients(char *msg)
{
    JRB ptr;
    pthread_mutex_lock(mut);
    jrb_traverse(ptr, current_clients)
    {
        Client cli = (Client) ptr.val.v;
        send_string(msg, cli->fd);
    }
    pthread_mutex_unlock(mut);
}

void communicate_with_client(void *v)
{
    int *fdp = (int*) v;
    int fd = *fdp;
    int msg_size;
    bool first = true;
    char buf[1100];
    int num_bytes;
    while ((num_bytes = read(fd, (void*) &msg_size, 4)) > 0)
    {
        time_t t = time(NULL);
        if (msg_size > 1099) 
        {
            fprintf(stderr, "Receive string: string too small (%d vs %d)\n", len, size);
            pthread_exit();
        }
        char *msg = buf;
        int i;
        while(msg < msg+msg_size) 
        {
            i = read(fd, msg, buf-msg+msg_size);
            if (i == 0) exit(0);
            if (i < 0) {
                perror("read");
                exit(1);
            }
            msg += i;
        }
        if (first)
        {
            add_client(buf, t, fd);
            first = false;
        }
        else
        {
            pthread_mutex_lock(mut);
            Client cli = jrb_find_str(current_clients_by_fd, fd);
            if (cli == NULL)
            {
                fprintf(stderr, "Error: couldn't find the client connected to file descriptor %d. Aborting.\n", fd);
                close(fd);
                pthread_exit();
            }
            cli->talk_time = ctime(t);
            pthread_mutex_unlock(mut);
        }
        send_message_to_clients(buf);
    }
    if (num_bytes != 0)
    {
        perror("Read error detected");
        close(fd);
        pthread_exit();
    }
    char *name;
    pthread_mutex_lock(mut);
    JRB node = jrb_find_str(current_clients_by_fd, fd);
    Client cli = (Client) node.val.v;
    name = (char*) malloc(strlen(cli->username) + 11);
    name = strcpy(cli->username);
    jrb_delete_node(node);
    free_client(cli);
    pthread_mutex_unlock(mut);
    close(fd)
    strcat(name, " has quit\n");
    send_message_to_clients(name);
    pthread_exit();
}

void accept_client_connections(void *v)
{
    int *sock_ptr = (int*) v;
    int sock = *sock_ptr;
    while (1)
    {
        int fd = accept_connection(sock);
        pthread_mutex_lock(mut);
        if (pthread_create(threads[num_connections+1], NULL, communicate_with_client, (void*) &fd) != 0)
        {
            perror("Could not create communication thread.");
            close(fd);
        }
        pthread_mutex_unlock(mut);
    }
}
