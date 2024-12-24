#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <future>
#include "CoreMacro.h"
#include "ThreadManager.h"

#include <WinSock2.h>
#include <mswsock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
    // WSAStartup(): winsock 초기화 (ws2_32 라이브러리로 초기화). 초기화 정보가 wsaData에 채워진다.
    WSAData wsaData;                                    
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)    
        return 0;

    // socket(): 소켓 하나를 생성해 리턴한다.
    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Socket ErrorCode : " << errCode << endl;
        return 0;
    }

    // SOCKADDR_IN: AF_INET(IPv4) 버전의 전송 주소 및 포트를 지정
    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;                            // IPv4
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);           // localhost: 니가 알아서 해줘
    serverAddr.sin_port = htons(7777);                          // port : 7777

    // 안내원 폰 개통! 식당의 대표 번호
    if(::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Bind ErrorCode : " << errCode << endl;
        return 0;
    }

    // 영업 시작!
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

        // 손님 입장!
        char ipAddress[16];
        ::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
        cout << "Client Connected! IP = " << ipAddress << endl;

    }

    WSACleanup();                           // winsock을 종료한다. (안해도 문제가 생기진 않음)
}


