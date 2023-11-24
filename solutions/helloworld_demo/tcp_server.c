#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <Port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);

    int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock < 0) {
        perror("socket failed");
        return 1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind failed");
        close(serv_sock);
        return 1;
    }

    if (listen(serv_sock, 5) < 0) {
        perror("listen failed");
        close(serv_sock);
        return 1;
    }

    printf("Server is listening on port %d...\n", port);

    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
    if (clnt_sock < 0) {
        perror("accept failed");
        close(serv_sock);
        return 1;
    }

    char buffer[1024];
    int bytes_read;
    while ((bytes_read = recv(clnt_sock, buffer, 1024, 0)) > 0) {
	fprintf(stderr, "%s\n", buffer);
	break;
        send(clnt_sock, buffer, bytes_read, 0);
    }

    if (bytes_read < 0) {
        perror("recv failed");
    }

    close(clnt_sock);
    close(serv_sock);

    return 0;
}

