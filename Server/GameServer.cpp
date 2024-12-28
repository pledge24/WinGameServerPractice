#include "pch.h"
#include <iostream>
#include "CorePch.h"

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
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
        return 0;

    // ioctlsocket(): 소켓의 I/O 모드를 제어하는 함수
    // FIONBIO: 논-블로킹 방식으로 전환
    u_long on = 1;
    if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
        return 0;

    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    serverAddr.sin_port = ::htons(7777);

    if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        return 0;

    if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
        return 0;

    cout << "Accept" << '\n';

    SOCKADDR_IN clientAddr;
    int32 addrLen = sizeof(clientAddr);

    // Accept
    while (true)
    {
        SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET)
        {
            // 원래 블록했어야 했는데... 너가 논블로킹으로 하라며?
            if (::WSAGetLastError() == WSAEWOULDBLOCK)
                continue;
        }
    
        cout << "Client Connected!" << '\n';

        // 데이터 송수신 시작!
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
                break;
            }
            else if (recvLen == 0)
            {
                // 연결 끊김
                break;
            }

            cout << "Recv Data Len = " << recvLen << '\n';

            // Send
            while (true)
            {
                if (::send(clientSocket, recvBuffer, recvLen, 0) == SOCKET_ERROR)
                {
                    // 원래 블록했어야 했는데... 너가 논블로킹으로 하라며?
                    if (::WSAGetLastError() == WSAEWOULDBLOCK)
                        continue;

                    // Error
                    break;
                }

                cout << "Send Data Len = " << recvLen << '\n';
                break;
            }
        }
    }
    

    WSACleanup();
}


