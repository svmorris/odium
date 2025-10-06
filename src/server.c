#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "tmux_handler.h"

#define UPORT 31398
#define CONN_BACKLOG 10
#define BUFFER_SIZE 1024

void send_fd(int sock, int fd_to_send);
void start_server(int tcp_port);


int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <tcp_port>\n", argv[0]);
        exit(1);
    }
    int tcp_port = atoi(argv[1]);

    // check if we are already running on a tmux session
    if (getenv("TMUX") != NULL)
    {
        int expected_name_size = 8+1;
        char *tmux_name = calloc(expected_name_size, sizeof(char));
        if (tmux_name == NULL)
        {
            perror("Memory allocation failed");
            exit(-8);
        }
        tmux_get_name(tmux_name, expected_name_size);

        if (tmux_name[0] == '\0')
        {
            puts("tmux_get_name failed");
            exit(-9);
        }

        if (strcmp(tmux_name, TMUX_SESSION_NAME) != 0)
        {
            printf("Taking over current tmux shell.\n");
            printf("WARNING: Odium will work worse (and look less cool) when inside tmux.");
            if (!tmux_change_name())
                exit(-10);
        }
    }
    else
    {
        printf("%s\n", argv[0]);
        // suicide -> launch with tmux
        tmux_relaunch(argc, argv);
    }

    tmux_set_pane_name("");

    puts(SPLASH);

    start_server(tcp_port);
    return 0;
}


/*
 * Start both a regular socket server
 * and an internal unix socket server
 */
void start_server(int tcp_port)
{
    int server_sock;
    int *client_sock;
    int u_server_sock;
    int u_client_sock;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    // ====================== Setup the internal unix socket server ======================
    // Unlink the socket path if it already exists
    unlink(SOCK_PATH);

    u_server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (u_server_sock < 0) {
        perror("Failed creating internal unix socket");
        exit(-12);
    }

    struct sockaddr_un uaddr = {0};
    uaddr.sun_family = AF_UNIX;
    strncpy(uaddr.sun_path, SOCK_PATH, sizeof(uaddr.sun_path)-1);

    if (bind(u_server_sock, (struct sockaddr*)&uaddr, sizeof(uaddr)) < 0)
    {
        perror("Failed to bind internal unix socket");
        exit(-13);
    }
    if (listen(u_server_sock, CONN_BACKLOG) < 0)
    {
        perror("Failed to set up internal socket for listening");
        exit(-14);
    }




    // ====================== Setup the regular socket server ======================
    // create the socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Failed to create the server socket");
        exit(-1);
    }

    // Server setup
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(tcp_port);

    // reuse socket ports
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

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

    printf("Started listening on port %d...\n", tcp_port);

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

        // Launch the client window
        tmux_new_pane();

        printf("Connection received from %s:%d\n",
                inet_ntoa(client_addr.sin_addr),
                ntohs(client_addr.sin_port));


        // Wait for the UI to connect back to receive
        // its socket address
        u_client_sock = accept(u_server_sock, NULL, NULL);
        if (u_client_sock < 0)
        {
            perror("Failed accepting incoming unix socket connetion");
            close(*client_sock);
            free(client_sock);
            continue;
        }

        send_fd(u_client_sock, *client_sock);

        close(u_client_sock);
        close(*client_sock);
        free(client_sock);
    }

    close(server_sock);
}



void send_fd(int sock, int fd_to_send)
{
    struct msghdr msg = {0};
    char buffer[CMSG_SPACE(sizeof(fd_to_send))];
    memset(buffer, 0, sizeof(buffer));


    // Normal payload (not control data). sendmsg requires some data,
    // so we send a dummy byte "X".
    struct iovec io = { .iov_base = (void*)"X", .iov_len = 1 };

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;

    msg.msg_control = buffer;
    msg.msg_controllen = sizeof(buffer);


    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;       // Control message is at the socket API level
    cmsg->cmsg_type  = SCM_RIGHTS;       // We're passing rights to a file descriptor
    cmsg->cmsg_len   = CMSG_LEN(sizeof(fd_to_send));

    // Copy the fd itself into the control message payload
    memcpy(CMSG_DATA(cmsg), &fd_to_send, sizeof(fd_to_send));

    // Actually send the message (fd + dummy data)
    if (sendmsg(sock, &msg, 0) < 0) {
        perror("Error occured while sending fd though internal socket");
        exit(-13);
    }
}
