#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "jrb.h"

static unsigned int num_connections = 0;
static JRB current_clients = make_jrb();
static JRB all_clients = make_jrb();

enum client_status_t
{
    active = "LIVE",
    disconnected = "DEAD",
};

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
    client_status_t stat;
} *Client;

Client new_client(char *join_message);

void print_client(Client cli, bool print_creation);

void add_client(char *join_message);

void print_current_clients();

void print_all_clients();

void jtalk_console();
