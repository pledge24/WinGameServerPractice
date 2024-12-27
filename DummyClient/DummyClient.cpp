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
    WSAData wsaData;                                    
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)    
        return 0;

    SOCKET clientSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        HandleError("Socket");
        return 0;
    }

    SOCKADDR_IN serverAddr; 
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;                            // IPv4
    ::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);    // localhost(사람이 읽을 수 있는 텍스트 표현의 IP주소를 이진 표현으로 변환해주는 함수)
    serverAddr.sin_port = ::htons(7777);                        // port : 7777

    while (true)
    {
        // 데이터 송신
        char sendBuffer[100] = "Hello World!";

        int32 resultCode = ::sendto(clientSocket, sendBuffer, sizeof(sendBuffer), 0,
            (SOCKADDR*)&serverAddr, sizeof(serverAddr));

        if (resultCode == SOCKET_ERROR)
        {
            HandleError("sendto");
            return 0;
        }
        
        cout << "Send Data! Data = " << sendBuffer << '\n';
        cout << "Send Data! Len = " << sizeof(sendBuffer) << '\n';

        // 데이터 수신
        char recvBuffer[1000];

        SOCKADDR_IN recvAddr;
        ::memset(&recvAddr, 0, sizeof(recvAddr));
        int32 addrLen = sizeof(recvAddr);

        int32 recvLen = ::recvfrom(clientSocket, recvBuffer, sizeof(recvBuffer), 0,
            (SOCKADDR*)&recvAddr, &addrLen);

        if (recvLen <= 0)
        {
            HandleError("RecvFrom");
            return 0;
        }

        cout << "Recv Data! Data = " << recvBuffer << '\n';
        cout << "Recv Data! Len = " << recvLen << '\n';

        this_thread::sleep_for(1s);
    }

    ::closesocket(clientSocket);

    WSACleanup();                           
}


