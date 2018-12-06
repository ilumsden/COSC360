#include "server_tasks.h"
#include "socketfun.h"

int main(int argc, char **argv)
{
    int sock;
    if (argc != 3) 
    {
        fprintf(stderr, "usage: jtalk_server host port\n");
        exit(1);
    }
    sock = serve_socket(argv[1], atoi(argv[2]));
    if (pthread_create(threads[0], NULL, accept_client_connections, (void*) &sock) != 0)
    {
        perror("Could not create acceptance thread.");
        shutdown(sock);
    }
    jtalk_console();
    shutdown(sock);
}
