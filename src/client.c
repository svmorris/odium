/*
 * User interface for accepted revshells
 * NOTE: this must be run by the server to function correctly
 */

#define _POSIX_C_SOURCE 200809L

#include <poll.h>
#include <poll.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <stdbool.h>
#include <termios.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "client.h"
#include "tmux_handler.h"

int recv_fd(int sock);
static void terminal_setup(void);
int get_peer_address(int fd, char *ip_str, size_t ip_str_len);

int client_main()
{
    int usock;
    int client_fd; // fd of the client socket gotten from server
    struct sockaddr_un uaddr = {0};


    if ((usock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("Failed creating internal socket on the client side");
        exit(-2);
    }


    uaddr.sun_family = AF_UNIX;
    strncpy(uaddr.sun_path, SOCK_PATH, sizeof(uaddr.sun_path)-1);


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


    // Set the client_fd socket to non-blocking
    int flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);


    // Start the terminal
    terminal_setup();

    struct pollfd pfd = {
        .fd = STDIN_FILENO,
        .events = POLLIN
    };

    ssize_t len = 0;
    char buffer[CLIENT_BUFFER_SIZE];
    unsigned char line_buffer[512];


    // The main loop currently crashes when it lopps
    // This is something to do with the keyuboard inputs
    while (true)
    {
        int ret = poll(&pfd, 1, 50);

        if (ret == -1)
        {
            // we can ignore this error
            if (errno == EINTR)
                continue;

            perror("Poll failed! Perhaps something is wrong with keyboard input");
            break;
        }

        if ( ret > 0 && (pfd.revents & POLLIN))
        {
            unsigned char input_buffer[256];
            ssize_t bytes_read = read(STDIN_FILENO, input_buffer, sizeof(input_buffer));

            if ( bytes_read > 0 )
            {
                for (ssize_t i = 0; i < bytes_read; i++)
                {
                    // Buffer one line before sending
                    unsigned char c = input_buffer[i];
                    line_buffer[len++] = c;

                    if (c == '\n' || len == sizeof(line_buffer))
                    {
                        write(client_fd, line_buffer, len);
                        len = 0;
                    }

                    // If there is a need for special per character logic,
                    // should be here.
                }
            }
        }



        ssize_t n = read(client_fd, buffer, CLIENT_BUFFER_SIZE);
        if (n > 0)
        {
            write(STDOUT_FILENO, buffer, n);
        }
        else if (n == 0)
        {
            // client closed connection
            puts("Connection closed from client\n");
            break;
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // Got no data
        }
        else
        {
            perror("Failed to read from the client socket. Likely closed");
            break;
        }
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


/*
 * The terminal needs to be setup so it
 * does not use the default key-input management
 * of the terminal.
 *
 * This is important to make read() non-blocking,
 * but also will help later when I add handling for
 * up arrow and ctrl+c
 */
static  void terminal_setup(void)
{
    struct termios t;

    // Get current terminal state
    tcgetattr(STDIN_FILENO, &t);

    // Switch to non-canonical mode
    t.c_lflag &= ~ICANON;


    // Make sure echo is on
    t.c_lflag |= ECHO;

    // input returns immediately and without delay
    t.c_cc[VMIN]  = 0;
    t.c_cc[VTIME] = 0;

    // Apply changes
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    // Mark the STDIN fd to be non-blocking
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}
