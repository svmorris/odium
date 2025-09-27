#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 31338
#define CONN_BACKLOG 10
#define BUFFER_SIZE 1024


void *handle_connection(void *sock);


int main()
{
    int server_sock;
    int *client_sock;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    // create the socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Failed to create the server socket");
        exit(-1);
    }

    // Server setup
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    // Bind the server socket
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Failed to bind the server socket");
        close(server_sock);
        exit(-2);
    }


    // Start listening
    if (listen(server_sock, CONN_BACKLOG) < 0)
    {
        perror("Failed to setup socket for listening");
        close(server_sock);
        exit(-3);
    }

    printf("Started listening on port %d...\n", PORT);

    while (1)
    {
        client_sock = malloc(sizeof(int));

        if (client_sock == NULL)
        {
            perror("Memory allocation failed while preparing to accept socket connection");
            continue;
        }

        // Accept the connection and get a new port from the kernel
        *client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size);
        if (*client_sock < 0)
        {
            perror("Failed to accept incoming connection");
            free(client_sock);
            continue;
        }

        printf("Connection received from %s:%d\n",
                inet_ntoa(client_addr.sin_addr),
                ntohs(client_addr.sin_port));


        // Create a new thread for each client
        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_connection, client_sock) != 0)
        {
            perror("Error creating new thread to handle client connection");
            close(*client_sock);
            free(client_sock);
        }
        else
        {
            // Let the thread loose
            pthread_detach(tid);
        }
    }

    close(server_sock);
    return 0;
}





void *handle_connection(void *sock)
{
    int client_sock = *(int *)sock;
    free(sock);

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t bytes_read;


    while ((bytes_read = recv(client_sock, buffer, BUFFER_SIZE -1, 0)) > 0)
    {
        buffer[bytes_read] = '\0';
        printf("c: %s\n", buffer);

        // just echo
        send(client_sock, buffer, bytes_read, 0);
    }

    close(client_sock);
    puts("Client disconnected\n");
    return NULL;
}
