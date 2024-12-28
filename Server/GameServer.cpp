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

void HandleError(const char* cause)
{
    int32 errCode = ::WSAGetLastError();
    cout << cause << " ErrorCode : " << errCode << '\n';
}

const int32 BUFSIZE = 1000;

struct Session
{
    SOCKET socket;
    char recvBuffer[BUFSIZE] = {};
    int32 recvBytes = 0;
    int32 sendBytes = 0;
};

int main()
{
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    // socket(): 소켓 하나를 생성해 리턴한다.
    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
    {
        HandleError("Socket");
        return 0;
    }

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

    if(::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
        return 0;

    cout << "Accept" << '\n';

    // Select 모델 = select 함수가 핵심이 되는 모델

    vector<Session> sessions;
    sessions.reserve(100);

    fd_set reads;
    fd_set writes;

    while (true)
    {
        // 소켓 셋 초기화
        FD_ZERO(&reads);
        FD_ZERO(&writes);

        // ListenSocket 등록
        FD_SET(listenSocket, &reads);

        // 나머지 소켓 등록
        for (Session& s : sessions)
        {
            // echo 서버이므로 recv -> send 순으로 실행됨
            // 데이터를 보낸 다음에 받기 위한 방법.
            if (s.recvBytes <= s.sendBytes)
                FD_SET(s.socket, &reads);
            else
                FD_SET(s.socket, &writes);
        }

        int32 retVal = ::select(0, &reads, &writes, nullptr, nullptr);
        if (retVal == SOCKET_ERROR)
            break;

        // Listener 소켓 체크
        if (FD_ISSET(listenSocket, &reads))
        {
            SOCKADDR_IN clientAddr;
            int32 addrLen = sizeof(clientAddr);
            SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
            if (clientSocket != INVALID_SOCKET)
            {
                cout << "Client Connected" << '\n';
                sessions.push_back({clientSocket});
            }
        }

        // 나머지 소켓 체크
        for (Session& s : sessions)
        {
            // Read
            if (FD_ISSET(s.socket, &reads))
            {
                int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
                if (recvLen <= 0)
                {
                    // TODO: sessions 제거
                    continue;
                }

                s.recvBytes = recvLen;
            }

            // Write
            if (FD_ISSET(s.socket, &writes))
            {
                // 블로킹 모드 -> 모든 데이터 다 보냄
                // 논블로킹 모드 -> 일부만 보낼 수가 있음 (상대방 수신 버퍼 상황에 따라)
                int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
                if (sendLen == SOCKET_ERROR)
                {
                    // TODO: sessions 제거
                    continue; 
                }

                s.sendBytes += sendLen;
                if (s.recvBytes == s.sendBytes)
                {
                    s.recvBytes = 0;
                    s.sendBytes = 0;
                }
            }
        }
    }


    ::WSACleanup();
}


