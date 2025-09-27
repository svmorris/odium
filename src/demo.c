#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 31338
#define BUFFER_SIZE 1024


void handle_connection(int socket_fd);

int main()
{
    // define stuff
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create the socket fd
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Creating socket failed");
        exit(-2);
    }

    // Make sure we can re-use the socket
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind socket to port
    // Accept only connections from localhost
    address.sin_family = AF_INET;
    /* address.sin_addr.s_addr = INADDR_ANY; */
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Failed binding socket to port");
        close(server_fd);
        exit(-3);
    }


    if (listen(server_fd, 3) < 0)
    {
        perror("Failed to start listener on socket");
        close(server_fd);
        exit(-4);
    }

    const char *hello = "hello from server";
    printf("Started listening on port %d...\n", PORT);

    while (1)
    {
        // Accept a client connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0) {
            perror("accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        printf("Accepted connection from %s:%d\n",
                inet_ntoa(address.sin_addr),
                ntohs(address.sin_port));

        handle_connection(new_socket);
    }

    // Clean up
    close(new_socket);
    close(server_fd);
    free(buffer);
}


void handle_connection(int socket_fd)
{

    char *buffer = malloc(BUFFER_SIZE);

    // 0 the buffer
    if (!buffer)
    {
        perror("Failed to allocate request buffer on the heap");
        exit(-1);
    }
    memset(buffer, 0, BUFFER_SIZE);


    // // Read data from client
    // int valread = read(new_socket, buffer, BUFFER_SIZE - 1);
    // if (valread > 0) {
    //     buffer[valread] = '\0';  // null terminate the string
    //     printf("Client says: %s\n", buffer);
    // }

    // // Send response to client
    // send(new_socket, hello, strlen(hello), 0);
    // printf("Hello message sent\n");

}
