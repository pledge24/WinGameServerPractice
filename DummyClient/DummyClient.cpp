#include "pch.h"
#include <iostream>

#include <WinSock2.h>
#include <mswsock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* cause)
{
    int32 errCode = ::WSAGetLastError();
    cout << cause << " ErrorCode : " << errCode << '\n';
}

int main()
{
    this_thread::sleep_for(1s);

    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
        return 0;

    u_long on = 1;
    if (::ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET)
        return 0;

    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    ::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    serverAddr.sin_port = ::htons(7777);

    // Connect
    while (true)
    {
        if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            // 원래 블록했어야 했는데... 너가 논블로킹으로 하라며?
            if (::WSAGetLastError() == WSAEWOULDBLOCK)
                continue;

            // 이미 연결된 상태라면 break
            if (::WSAGetLastError() == WSAEISCONN)
                break;

            break;
        }
    }

    cout << "Connected to Server!" << '\n';

    while (true)
    {
        char sendBuffer[100] = "Hello World";

        // Echoing
        while (true)
        {
            // Send
            while (true)
            {
                if (::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
                {
                    // 원래 블록했어야 했는데... 너가 논블로킹으로 하라며?
                    if (::WSAGetLastError() == WSAEWOULDBLOCK)
                        continue;

                    // Error!
                    // 에러 처리 코드...
                    break;
                }

                cout << "Send Data! Len= " << sizeof(sendBuffer) << '\n';
                break;
            }

            // Recv
            while (true)
            {
                char recvBuffer[1000];
                int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
                if (recvLen == SOCKET_ERROR)
                {
                    // 원래 블록했어야 했는데... 너가 논블로킹으로 하라며?
                    if (::WSAGetLastError() == WSAEWOULDBLOCK)
                        continue;

                    // Error
                    // 에러 처리 코드...
                    break;
                }
                else if (recvLen == 0)
                {
                    // 연결 끊김
                    break;
                }

                cout << "Recv Data! Len= " << recvLen << '\n';
                break;
            }

            this_thread::sleep_for(1s);
        }
    }

    // 소켓 리소스 반환
    ::closesocket(clientSocket);

    // 윈속 종료
    ::WSACleanup();
}

