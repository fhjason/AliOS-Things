#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static void udp_echo_client(int argc, char **argv) {
    if (argc < 3) {
        printf("invalid arguments\r\n");
        return;
    }

    char *server_ip = argv[1];
    unsigned short port = atoi(argv[2]);

    // Create a UDP socket
    int clntsock;
    if ((clntsock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("socket failed\r\n");
        return;
    }

    // Construct server address structure
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(server_ip);
    servaddr.sin_port = htons(port);

    char buf[1024];
    printf("Enter a message: ");
    fgets(buf, sizeof(buf), stdin);

    // Send the message to the server
    int len = strlen(buf);
    if (sendto(clntsock, buf, len, 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) != len) {
        printf("sendto failed\r\n");
        close(clntsock);
        return;
    }

    // Receive a response from the server
    struct sockaddr_in fromaddr;
    socklen_t fromlen = sizeof(fromaddr);
    int r = recvfrom(clntsock, buf, sizeof(buf), 0, (struct sockaddr *) &fromaddr, &fromlen);
    if (r < 0) {
        printf("recvfrom failed\r\n");
    } else {
        buf[r] = '\0'; // Null-terminate the received string
        printf("Received: %s", buf);
    }

    close(clntsock);
}

