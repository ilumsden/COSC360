#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "jrb.h"

// Defines the maximum number of allowed threads
#define MAX_THREADS 1000

// The host name passed through the command line.
// Stored for console printing purposes.
char *host;
// The port passed through the command line.
// Stored for console printing purposes.
int port;

// threads is used to close all spawned threads prior to server close.
pthread_t threads[MAX_THREADS];
// The number of active connections
unsigned int num_connections;
// The number of clients who have ever connected to the server
unsigned int num_clients;
// A red-black tree for storing the currently-connected clients
// It is keyed on the client's connection IDs
JRB current_clients;
// A red-black tree for storing all clients, active or not
// It is keyed on the client's connection IDs
JRB all_clients;
// A mutex to protect operations on the other globals
pthread_mutex_t *mut;

// Sends len bytes in p to the client represented by fd
void send_bytes_server(char *p, int len, int fd);

// Sends string s to the client represented by fd
void send_string_server(char *s, int fd);

// Closes all active threads, frees the Client structs, and exits the process.
void shutdown(int sock);

// Client is a struct for storing all the needed data for a client.
typedef struct client_t
{
    // The client's name
    char *username;
    // The connection ID for the client
    unsigned int connection_id;
    // Note: none of these time strings need to be freed.
    // The are acutally char buffers.
    // Also, they contain a newline character already.
    
    // A string representing the time of connection for the client.
    char *creation_time;
    // A string representing the time that the client last sent a message.
    char *talk_time;
    // A string representing the time when the client disconnected from the server
    char *quit_time;
    // A string representing whether or not the client is actively connected to the server.
    // It can be either LIVE or DEAD.
    char *stat;
    // The file descriptor for reading from/writing to the client.
    int fd;
} *Client;

// Creates a new Client using their joining message, the time that message was sent, and the file
// descriptor for the client.
Client new_client(char *join_message, time_t t, int fd);

// Prints the data in cli.
// print_creation is used to control whether the printing is for an ALL command or a
// TALKERS command.
void print_client(Client cli, bool print_creation);

// Creates a new Client and adds it to the JRBs.
// Returns the new Client's connection ID.
int add_client(char *join_message, time_t t, int fd);

// Frees the memory of the passed Client
void free_client(Client cli);

// Prints the currently-connected clients
// Run when the console receives the TALKERS command.
void print_current_clients();

// Prints all clients
// Run when the console receives the ALL command.
void print_all_clients();

// Controls the JTALK console
void jtalk_console();

// Sends the passed message to all connected clients.
void send_message_to_clients(char *msg);

// Controls reading from and writing to a specific client.
// v is actually the file pointer for the client.
// This function is supposed to be run through a pthread, hence the typing.
void* communicate_with_client(void *v);

// Controls accepting clients.
// v is actually the file pointer for the socket.
// This function is supposed to be run through a pthread, hence the typing.
void* accept_client_connections(void *v);
