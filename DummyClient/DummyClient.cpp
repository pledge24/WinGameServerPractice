#include "pch.h"
#include <iostream>

#include <WinSock2.h>
#include <mswsock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{ 
    WSAData wsaData;                                    
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)    
        return 0;

    SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Socket ErrorCode : " << errCode << endl;
        return 0;
    }

    SOCKADDR_IN serverAddr; 
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;                            // IPv4
    ::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);    // localhost(사람이 읽을 수 있는 텍스트 표현의 IP주소를 이진 표현으로 변환해주는 함수)
    serverAddr.sin_port = ::htons(7777);                        // port : 7777

    if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Connect ErrorCode : " << errCode << endl;
        return 0;
    }

    // 연결 성공! 데이터 송수신 가능!
    cout << "Connection is sucess" << endl;

    while (true)
    {
        // 데이터 송신
        char sendBuffer[1000] = "Hello World!";

        int32 resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
        if (resultCode == SOCKET_ERROR)
        {
            int32 errCode = ::WSAGetLastError();
            cout << "Send ErrorCode : " << errCode << endl;
            return 0;
        }
        
        cout << "Send Data! Data = " << sendBuffer << '\n';
        cout << "Send Data! Len = " << sizeof(sendBuffer) << '\n';

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

        this_thread::sleep_for(1s);
    }

    ::closesocket(clientSocket);

    WSACleanup();                           
}


