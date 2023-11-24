import socket

def udp_server(host='0.0.0.0', port=12345):
    """
    一个简单的 UDP 服务器
    :param host: 监听的地址
    :param port: 监听的端口号
    """
    # 创建一个 UDP 套接字
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        # 绑定套接字到指定地址和端口
        s.bind((host, port))
        print(f"UDP Server listening on {host}:{port}")

        while True:
            # 接收数据
            data, addr = s.recvfrom(1024)  # 缓冲区大小设置为 1024 字节
            print(f"Received message: '{data.decode()}' from {addr}")

# 运行服务器
udp_server()

