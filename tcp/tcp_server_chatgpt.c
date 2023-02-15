#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define MAX_EVENTS 20
#define BUF_SIZE 2000

int set_nonblocking(int fd) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(int argc, char* argv[]) {
    int server_socket, client_socket, epoll_fd, event_count, read_size;
    struct sockaddr_in server_address, client_address;
    char client_message[BUF_SIZE];
    struct epoll_event event, events[MAX_EVENTS];

    // 创建服务器套接字
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // 设置服务器地址和端口
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    // 绑定端口
    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));

    // 监听连接
    listen(server_socket, SOMAXCONN);

    printf("Server started listening on port 8080\n");

    // 创建 epoll 实例
    epoll_fd = epoll_create1(0);
    event.events = EPOLLIN;
    event.data.fd = server_socket;

    // 将服务器套接字添加到 epoll 实例中
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &event);

    while (1) {
        // 等待事件发生
        event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        // 处理所有就绪的事件
        for (int i = 0; i < event_count; i++) {
            // 如果是新连接事件
            if (events[i].data.fd == server_socket) {
                socklen_t address_len = sizeof(client_address);
                client_socket = accept(server_socket, (struct sockaddr*)&client_address, &address_len);

                printf("Client connected with IP address: %s\n", inet_ntoa(client_address.sin_addr));

                // 将新连接套接字添加到 epoll 实例中
                set_nonblocking(client_socket);
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = client_socket;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event);
            }
            // 如果是数据可读事件
            else {
                client_socket = events[i].data.fd;
                read_size = recv(client_socket, client_message, BUF_SIZE, 0);

                if (read_size <= 0) {
                    // 关闭连接并从 epoll 实例中删除
                    close(client_socket);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_socket, NULL);
                    continue;
                }

                // 处理客户端请求
                write(client_socket, "Received message\n", 18);
                printf("Received message: %s\n", client_message);
            }
        }
    }

    return 0;
}
