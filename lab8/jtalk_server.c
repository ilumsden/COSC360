#include "server_tasks.h"
#include "socketfun.h"

// These globals are defined in server_tasks.h
extern char *host;
extern int port;

extern unsigned int num_clients;
extern unsigned int num_connections;
extern JRB current_clients;
extern JRB all_clients;
extern pthread_mutex_t *mut;
extern pthread_t threads[MAX_THREADS];

int main(int argc, char **argv)
{
    // Initialize the globals
    pthread_mutex_t m;
    mut = &m;
    num_connections = 0;
    num_clients = 0;
    current_clients = make_jrb();
    all_clients = make_jrb();
    pthread_mutex_init(mut, NULL);
    int sock;
    // Makes sure the correct command line arguments were passed.
    if (argc != 3) 
    {
        fprintf(stderr, "usage: jtalk_server host port\n");
        exit(1);
    }
    // Sets more globals and serves a socket.
    host = strdup(argv[1]);
    port = atoi(argv[2]);
    sock = serve_socket(argv[1], atoi(argv[2]));
    // Creates a new thread to control accepting connections.
    pthread_mutex_lock(mut);
    if (pthread_create(&(threads[0]), NULL, accept_client_connections, (void*) &sock) != 0)
    {
        perror("Could not create acceptance thread.");
        shutdown(sock);
    }
    pthread_mutex_unlock(mut);
    // Runs the Jtalk console on the main thread.
    jtalk_console();
    // When stdin closes, shutdown the server.
    shutdown(sock);
}
