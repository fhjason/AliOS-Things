/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/init.h"
#include "board.h"
#include <aos/errno.h>
#include <aos/kernel.h>
#include <aos/cli.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

int application_start(int argc, char *argv[])
{
    int count = 0;

    printf("nano entry here!\r\n");

    while (1) {
        printf("hello world! count %d \r\n", count++);
        aos_msleep(10000);
    };
}
static int tcp_client(int argc, char **argv)
{
    if (argc <= 3) {
        printf("Usage: %s <Server IP> <Port> <String>\n", argv[0]);
        return 1;
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    // 创建一个 socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket failed");
        return 1;
    }

    // 构建服务器地址结构
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // 将 IP 地址从字符串转换为二进制形式
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return 1;
    }

    // 连接到服务器
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return 1;
    }

    char buffer[1024] = {0};

    // 发送数据
    fprintf(stderr, "Message befor sent\n");
    // send(sock, argv[3], sntrlen(argv[3])+1, 0);
    if (send(sock, argv[3], 3, 0) != strlen(argv[3])) {
                printf("ERR: send failed\r\n");
            }
    fprintf(stderr, "Message sent\n");

    // 接收来自服务器的响应
    int bytes_read = recv(sock, buffer, 10, 0);
    if (bytes_read > 0) {
        fprintf(stderr, "Server response: %s\n", buffer);
    }else{
        fprintf(stderr, "err::\n");
    }

    // 关闭 socket
    lwip_close(sock);

    return 0;
}

static void tcp_echo_server(int argc, char **argv)
{
    if (argc < 2) {
        printf("invalid arguments\r\n");
        return;
    }

    unsigned short port = atoi(argv[1]);

    // Create a listening socket (also called server socket) 

    int servsock;
    if ((servsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket failed\r\n");
        return;
    }

    // Construct local address structure

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // any network interface
    servaddr.sin_port = htons(port);

    // Bind to the local address

    if (bind(servsock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        printf("bind failed\r\n");
        lwip_close(servsock);
        return;
    }

    // Start listening for incoming connections

    if (listen(servsock, 5 /* queue size for connection requests */ ) < 0) {
        printf("listen failed\r\n");
        lwip_close(servsock);
        return;
    }

    int r;
    char buf[10];

    int clntsock;
    socklen_t clntlen;
    struct sockaddr_in clntaddr;

    while (1) {

        // Accept an incoming connection

        printf("waiting for client...\r\n");

        clntlen = sizeof(clntaddr); // initialize the in-out parameter

        if ((clntsock = accept(servsock, (struct sockaddr *) &clntaddr, &clntlen)) < 0) {
            printf("accept failed\r\n");
            lwip_close(servsock);
            return;
        }

        // accept() returned a connected socket (also called client socket)
        // and filled in the client's address into clntaddr

        printf("client ip: %s\r\n", inet_ntoa(clntaddr.sin_addr));

        // Receive msg from client, capitalize the 1st char, send it back

        while ((r = recv(clntsock, buf, sizeof(buf), 0)) > 0) {
            if (send(clntsock, buf, r, 0) != r) {
                printf("ERR: send failed\r\n");
                break;
            }
        }

        if (r < 0) {
            printf("ERR: recv failed\r\n");
        } else if (r == 0) {
            printf("closed by client\r\n");
        }

        // Client closed the connection (r==0) or there was an error
        // Either way, close the client socket and go back to accept()

        lwip_close(clntsock);
    }
}

static void udp_echo_client(int argc, char **argv) {
    if (argc < 4) {
        printf("Usage: %s <Server IP> <Port> <String>\n", argv[0]);
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

    // Send the message to the server
    int len = strlen(argv[3]);
    if (sendto(clntsock, argv[3], len, 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) != len) {
        printf("sendto failed\r\n");
        lwip_close(clntsock);
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
        fprintf(stderr, "Received: %s", buf);
    }

    lwip_close(clntsock);
}


ALIOS_CLI_CMD_REGISTER(tcp_echo_server, tcp_echo_server, tcp echo server)
ALIOS_CLI_CMD_REGISTER(udp_echo_client, udp_echo_client, udp echo client)
ALIOS_CLI_CMD_REGISTER(tcp_client, tcp_client, tcp client)
