#include "pch.h"
#include <iostream>

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
    SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Socket ErrorCode : " << errCode << endl;
        return 0;
    }

    // SOCKADDR_IN: AF_INET(IPv4) 버전의 전송 주소 및 포트를 지정
    SOCKADDR_IN serverAddr; 
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;                            // IPv4
    // serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1");   // localhost -deprecated-
    ::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);    // localhost(사람이 읽을 수 있는 텍스트 표현의 IP주소를 이진 표현으로 변환해주는 함수)
    serverAddr.sin_port = htons(7777);                          // port : 7777

    // Connect(): 연결 요청
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
        this_thread::sleep_for(1s);
    }

    // 소켓 리소스 반환
    ::closesocket(clientSocket);

    WSACleanup();                           // winsock을 종료한다. (안해도 문제가 생기진 않음) 
}


