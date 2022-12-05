#include <arpa/inet.h>
#include <errno.h>
// #include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
// #include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// https://zhuanlan.zhihu.com/p/35367402

void work() {
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("listen socket");
        _exit(-1);
    }
    int ret = 0;
    int reuse = 1;
    ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&reuse, sizeof(int));
    if (ret < 0) {
        perror("setsockopt");
        _exit(-1);
    }
    ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, (const void *)&reuse, sizeof(int));
    if (ret < 0) {
        perror("setsockopt");
        _exit(-1);
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    // addr.sin_addr.s_addr = inet_addr("10.95.118.221");
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    addr.sin_port = htons(9980);
    ret = bind(listenfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        perror("bind addr");
        _exit(-1);
    }
    printf("bind success\n");
    ret = listen(listenfd, 10);
    if (ret < 0) {
        perror("listen");
        _exit(-1);
    }
    printf("listen success\n");
    struct sockaddr clientaddr;
    int len = 0;
    while (1) {
        printf("process:%d accept...\n", getpid());
        int clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, &len);
        if (clientfd < 0) {
            printf("accept:%d %s", getpid(), strerror(errno));
            _exit(-1);
        }
        close(clientfd);
        printf("process:%d close socket\n", getpid());
    }
}
int main() {
    printf("uid:%d euid:%d\n", getuid(), geteuid());
    int i = 0;
    for (i = 0; i < 6; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            work();
        }
        if (pid < 0) {
            perror("fork");
            continue;
        }
    }
    int status, id;
    while ((id = waitpid(-1, &status, 0)) > 0) {
        printf("%d exit\n", id);
    }
    if (errno == ECHILD) {
        printf("all child exit\n");
    }
    return 0;
}