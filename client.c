#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

void replace_newline_with_underscore(char s[])
{
    int i = 0;
    while (s[i] != '\0')
    {
        if (s[i] == '\n')
        {
            s[i] = '_';
        }
        i++;
    }
}

void wait_time()
{
    int remaining_time = 5; // 5 seconds countdown
    time_t start_time = time(NULL);

    while (remaining_time > 0)
    {
        printf("\rTime remaining: %d seconds", remaining_time);
        fflush(stdout);
        sleep(1);
        remaining_time--;
    }

    printf("\rYou are done! \n"); // clear remaining time and print final message
}

int main(int argc, char *argv[])
{
    const char *port = "65001"; // Port number for communication
    char *host;
    struct addrinfo hints, *server;
    int r, sockfd, done;
    char buffer[BUFSIZ];
    fd_set read_fd;

    int counter = 0;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]); // Print usage format
        exit(1);
    }
    host = argv[1];

    // Obtain and convert server name and port
    printf("\nConnecting to %s...", host);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    r = getaddrinfo(host, port, &hints, &server);
    if (r != 0)
    {
        perror("Failed to get address info");
        exit(1);
    }
    puts("\nConnection established!");
    puts("\nEnter your Name, Class and Roll no. [COURSE_DIV_NAME_NUMBER]");

    // Create a socket
    sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
    if (sockfd == -1)
    {
        perror("Failed to create socket");
        exit(1);
    }

    // Connect to the socket
    r = connect(sockfd, server->ai_addr, server->ai_addrlen);
    freeaddrinfo(server);
    if (r == -1)
    {
        perror("Failed to connect");
        exit(1);
    }

    // Loop to interact with the server
    done = FALSE;
    while (!done)
    {
        // Initialize file descriptor set
        FD_ZERO(&read_fd);
        FD_SET(sockfd, &read_fd); // Add the socket
        FD_SET(0, &read_fd);      // Add standard input

        r = select(sockfd + 1, &read_fd, NULL, NULL, 0);
        if (r == -1)
        {
            perror("Select failed");
            exit(1);
        }

        // Remote input
        if (FD_ISSET(sockfd, &read_fd))
        {
            sleep(1);
            r = recv(sockfd, buffer, BUFSIZ, 0);
            // Check for zero input, disconnection
            if (r < 1)
            {
                puts(" Connection closed by peer");
                break;
            }
            // Otherwise, cap the buffer and output it
            buffer[r] = '\0';
            printf("%s", buffer);
        }

        // Local input
        if (FD_ISSET(0, &read_fd))
        {
            // Don't send an empty line
            if (fgets(buffer, BUFSIZ, stdin) == NULL)
            {
                putchar('\n');
            }
            // If 'done' is input, close the loop and quit
            else if (strcmp(buffer, "close\n") == 0)
            {
                done = TRUE;
            }
            // Otherwise, send the input string - including the newline
            else
            {
                if (counter == 0)
                {
                    replace_newline_with_underscore(buffer);
                    send(sockfd, buffer, strlen(buffer), 0);
                    counter++;
                    printf("\nYour timer has started.");
                    wait_time();
                    replace_newline_with_underscore(buffer);
                    strcat(buffer, "Present\n");
                    send(sockfd, buffer, strlen(buffer), 0);
                    printf("\nYour Attendance has been marked...\n");
                    break;
                }
                else
                {
                    printf("Chat Disabled!!");
                }
            }
        }
    } // End while loop

    // Clean-up
    printf("\nDisconnected\n");
    close(sockfd);

    return (0);
}
