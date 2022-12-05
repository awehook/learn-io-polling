project(io-polling)

add_executable(epoll_socket_cmake epoll_socket.c common.c)

add_executable(server_128 server_128.c)