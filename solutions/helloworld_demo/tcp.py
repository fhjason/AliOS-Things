import socket

def tcp_client(host, port, message):
    """
    一个简单的 TCP 客户端来发送字符串消息
    :param host: 服务器的主机名或 IP 地址
    :param port: 服务器的端口号
    :param message: 要发送的字符串消息
    """
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        # 连接到服务器
        s.connect((host, port))
        # 发送消息
        s.sendall(message.encode())
        # 关闭连接
        s.close()

# 使用示例
tcp_client('10.0.2.15', 49153, 'Hello, World!')

