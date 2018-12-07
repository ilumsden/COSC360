#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "jrb.h"

#define MAX_THREADS 1000

pthread_t threads[MAX_THREADS];
unsigned int num_connections;
JRB current_clients;
JRB all_clients;
pthread_mutex_t *mut;

void send_bytes_server(char *p, int len, int fd);

void send_string_server(char *s, int fd);

void shutdown(int sock);

typedef struct client_t
{
    char *username;
    unsigned int connection_id;
    // Note: neither of these strings need to be freed.
    // The are acutally char buffers.
    // Also, they contain a newline character already.
    char *creation_time;
    char *talk_time;
    char *quit_time;
    char *stat;
    int fd;
} *Client;

Client new_client(char *join_message, time_t t, int fd);

void print_client(Client cli, bool print_creation);

int add_client(char *join_message, time_t t, int fd);

void free_client(Client cli);

void print_current_clients();

void print_all_clients();

void jtalk_console();

void send_message_to_clients(char *msg);

void* communicate_with_client(void *v);

void* accept_client_connections(void *v);
