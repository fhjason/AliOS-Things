#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <Server IP> <Port>\n", argv[0]);
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
    printf("Enter a message: ");
    fgets(buffer, 1024, stdin);

    // 发送数据
    send(sock, buffer, strlen(buffer), 0);
    printf("Message sent\n");

    // 接收来自服务器的响应
    int bytes_read = recv(sock, buffer, 1024, 0);
    if (bytes_read > 0) {
        printf("Server response: %s\n", buffer);
    }

    // 关闭 socket
    close(sock);

    return 0;
}

