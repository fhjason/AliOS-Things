#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

void main(int argc, char **argv) {
    if (argc < 2) {
        printf("invalid arguments\r\n");
        return;
    }

    unsigned short port = atoi(argv[1]);

    // Create a UDP socket
    int servsock;
    if ((servsock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("socket failed\r\n");
        return;
    }

    // Construct local address structure
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    // Bind to the local address
    if (bind(servsock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        printf("bind failed\r\n");
        close(servsock);
        return;
    }

    char buf[1024];
    struct sockaddr_in clntaddr;
    socklen_t clntlen = sizeof(clntaddr);

    while (1) {
	printf("waiting for client...\r\n");
        // Receive messages from clients
        int r = recvfrom(servsock, buf, sizeof(buf), 0,
                         (struct sockaddr *) &clntaddr, &clntlen);
        if (r < 0) {
            printf("recvfrom failed\r\n");
            continue;
        }

        fprintf(stderr, "Received message from %s: %s\r\n", inet_ntoa(clntaddr.sin_addr), buf);

        // Send the same message back to the client
        if (sendto(servsock, buf, r, 0, (struct sockaddr *) &clntaddr, clntlen) != r) {
            printf("sendto failed\r\n");
        }
    }
}

