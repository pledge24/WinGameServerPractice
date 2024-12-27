#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <WinSock2.h>
#include <mswsock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
    WSAData wsaData;                                    
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)    
        return 0;

    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Socket ErrorCode : " << errCode << endl;
        return 0;
    }

    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;                            // IPv4
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);           // localhost: 니가 알아서 해줘
    serverAddr.sin_port = ::htons(7777);                        // port : 7777

    if(::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Bind ErrorCode : " << errCode << endl;
        return 0;
    }

    if (::listen(listenSocket, 10) == SOCKET_ERROR)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Listen ErrorCode : " << errCode << endl;
        return 0;
    }

    cout << "open" << '\n';

    while (true)
    {
        SOCKADDR_IN clientAddr; 
        ::memset(&clientAddr, 0, sizeof(clientAddr));
        int32 addrLen = sizeof(clientAddr);

        SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET)
        {
            int32 errCode = ::WSAGetLastError();
            cout << "Accept ErrorCode : " << errCode << endl;
            return 0;
        }

        char ipAddress[16];
        ::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
        cout << "Client Connected! IP = " << ipAddress << endl;

        while (true)
        {
            // 데이터 수신
            char recvBuffer[1000];

            int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
            if (recvLen <= 0)
            {
                int32 errCode = ::WSAGetLastError();
                cout << "Recv ErrorCode : " << errCode << endl;
                return 0;
            }

            cout << "Recv Data! Data = " << recvBuffer << '\n';
            cout << "Recv Data! Len = " << sizeof(recvBuffer) << '\n';

            // 데이터 재전송
            int32 resultCode = ::send(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
            if (resultCode == SOCKET_ERROR)
            {
                int32 errCode = ::WSAGetLastError();
                cout << "Send ErrorCode : " << errCode << endl;
                return 0;
            }

            cout << "Send Data! Data = " << recvBuffer << '\n';
            cout << "Send Data! Len = " << sizeof(recvBuffer) << '\n';
        }

    }

    WSACleanup();                           
}


