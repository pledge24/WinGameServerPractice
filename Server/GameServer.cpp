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
    // Winsock 초기화
    WSAData wsaData;                                    
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)    
        return 0;

    // 소켓 생성
    SOCKET serverSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == SOCKET_ERROR)
    {
        HandleError("Socket");
        return 0;
    }

    // 서버 주소 생성
    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    serverAddr.sin_port = ::htons(7777);

    // 소켓과 서버 주소 엮기(바인딩)
    if (::bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        HandleError("Bind");
        return 0;
    }

    while (true)
    {
        SOCKADDR_IN clientAddr;
        ::memset(&clientAddr, 0, sizeof(clientAddr));
        int32 addrLen = sizeof(clientAddr);

        char recvBuffer[1000];

        // 데이터 수신
        int32 recvLen = ::recvfrom(serverSocket, recvBuffer, sizeof(recvBuffer), 0,
            (SOCKADDR*)&clientAddr, &addrLen);

        if (recvLen <= 0)
        {
            HandleError("RecvFrom");
            return 0;
        }

        cout << "Recv Data! Data = " << recvBuffer << '\n';
        cout << "Recv Data! Len = " << recvLen << '\n';

        // 데이터 송신
        int32 resultCode = ::sendto(serverSocket, recvBuffer, recvLen, 0,
            (SOCKADDR*)&clientAddr, sizeof(clientAddr));

        // 원래는 소켓이 에러났다고 이렇게 프로그램을 강제로 끄는 일은 있으면 안된다.
        if (resultCode == SOCKET_ERROR)
        {
            HandleError("sendto");
            return 0;
        }

        cout << "Send Data! Data = " << recvBuffer << '\n';
        cout << "Send Data! Len = " << recvLen << '\n';
    }

    WSACleanup();                           
}


