/*
 * User interface for accepted revshells
 * NOTE: this must be run by the server to function correctly
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <stdbool.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "tmux_handler.h"

# define BUFFER_SIZE 4096

int recv_fd(int sock);
int get_peer_address(int fd, char *ip_str, size_t ip_str_len);

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <unix_socket_path>\n", argv[0]);
        puts("This binary only functions correctly when called by the accompanying server. Are you sure you meant to run this?\n");
        exit(-1);
    }

    int usock;
    int client_fd; // fd of the client socket gotten from server
    struct sockaddr_un uaddr = {0};


    if ((usock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("Failed creating internal socket on the client side");
        exit(-2);
    }


    uaddr.sun_family = AF_UNIX;
    strncpy(uaddr.sun_path, argv[1], sizeof(uaddr.sun_path)-1);


    if (connect(usock, (struct sockaddr*)&uaddr, sizeof(uaddr)) < 0)
    {
        perror("Failed to connect to the internal socket on the client side");
        exit(-3);
    }

    // Get the client socket from the server
    client_fd = recv_fd(usock);
    close(usock);


    // Set the tmux pane name
    char *ip_str = malloc(INET6_ADDRSTRLEN);
    if (get_peer_address(client_fd, ip_str, INET6_ADDRSTRLEN) == 0)
        tmux_set_pane_name(ip_str);
    else
        tmux_set_pane_name("Unknown");

    // Simple (and temporary) blocking loop
    // In the future this should be:
    //  A: non-blocking: make sure the client can always receive when typing
    //  B: interactive shell via poll()/select()
    ssize_t n;
    char buffer[BUFFER_SIZE];
    while(1)
    {
        if ((n = read(client_fd, buffer, BUFFER_SIZE)) > 0)
            write(STDOUT_FILENO, buffer, n);

        if ((n = read(STDIN_FILENO, buffer, BUFFER_SIZE)) <= 0)
            break;

        write(client_fd, buffer, n);
    }

    close(client_fd);
    return 0;
}


// Get a string representation of the connected sockets IP address
int get_peer_address(int fd, char *ip_str, size_t ip_str_len)
{
     struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    if (getpeername(fd, (struct sockaddr*)&addr, &len) == -1)
        return -1;

    const void *src = NULL;

    if (addr.ss_family == AF_INET) {
        const struct sockaddr_in *s = (const struct sockaddr_in *)&addr;
        src = &s->sin_addr;
    } else if (addr.ss_family == AF_INET6) {
        const struct sockaddr_in6 *s = (const struct sockaddr_in6 *)&addr;
        src = &s->sin6_addr;
    } else {
        return -1;
    }

    if (inet_ntop(addr.ss_family, src, ip_str, ip_str_len) == NULL)
        return -1;

    return 0;
}

int recv_fd(int sock)
{
    struct msghdr msg = {0};
    char m_buffer[1]; // for the dummy data: 'x'
    struct iovec io = { .iov_base = m_buffer, .iov_len = sizeof(m_buffer)};


    char cmsgbuf[CMSG_SPACE(sizeof(int))];
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = sizeof(cmsgbuf);

    // Receive message (may include both normal + control data)
    if (recvmsg(sock, &msg, 0) < 0) {
        perror("Error occurred when receiving fd from server on client side");
        exit(-5);
    }

    // Extract first control message
    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    if (!cmsg || cmsg->cmsg_level != SOL_SOCKET || cmsg->cmsg_type != SCM_RIGHTS) {
        fprintf(stderr, "No file descriptor received!\n");
        exit(-6);
    }

    int fd;
    memcpy(&fd, CMSG_DATA(cmsg), sizeof(fd)); // Copy fd out of the control data
    return fd;
}
