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

    // WSAEventSelect 모델: WSAEventSelect 함수가 핵심이 된다.

    vector<WSAEVENT> wsaEvents;
    vector<Session> sessions;
    sessions.reserve(100);

    WSAEVENT listenEvent = ::WSACreateEvent();
    wsaEvents.push_back(listenEvent);
    sessions.push_back(Session{ listenSocket });
    if (::WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
        return 0;

    while (true)
    {
        int32 index = ::WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], FALSE, WSA_INFINITE, FALSE);
        if (index == WSA_WAIT_FAILED)
            continue;

        index -= WSA_WAIT_EVENT_0;

        //::WSAResetEvent(wsaEvents[index]);

        WSANETWORKEVENTS networkEvents;
        if(::WSAEnumNetworkEvents(sessions[index].socket, wsaEvents[index], &networkEvents) == SOCKET_ERROR)
            continue;

        // Listener 소켓 체크
        if (networkEvents.lNetworkEvents & FD_ACCEPT)
        {
            // Error-Check
            if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
                continue;

            SOCKADDR_IN clientAddr;
            int32 addrLen = sizeof(clientAddr);

            SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
            if (clientSocket != INVALID_SOCKET)
            {
                cout << "Client Connected" << '\n';

                WSAEVENT clientEvent = ::WSACreateEvent();
                wsaEvents.push_back(clientEvent);
                sessions.push_back(Session{ clientSocket });
                if (::WSAEventSelect(clientSocket, clientEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
                    return 0;
            }
        }
        
        // Client Session 소켓 체크
        if (networkEvents.lNetworkEvents & FD_READ || networkEvents.lNetworkEvents & FD_WRITE)
        {
            // Error-Check
            if ((networkEvents.lNetworkEvents & FD_READ) && (networkEvents.iErrorCode[FD_READ_BIT] != 0))
                continue;

            // Error-Check
            if ((networkEvents.lNetworkEvents & FD_WRITE) && (networkEvents.iErrorCode[FD_WRITE_BIT] != 0))
                continue;

            Session& s = sessions[index];

            // Read
            if (s.recvBytes == 0)
            {
                int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);

                if (recvLen == SOCKET_ERROR)
                {
                    if (::WSAGetLastError() != WSAEWOULDBLOCK)
                    {
                        // TODO : Remove Session
                    }
                    continue;
                }

                s.recvBytes = recvLen;
                cout << "Recv Data = " << recvLen << '\n';
            }

            // Write
            if (s.recvBytes > s.sendBytes)
            {
                int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);

                if (sendLen == SOCKET_ERROR)
                {
                    if (::WSAGetLastError() != WSAEWOULDBLOCK)
                    {
                        // TODO : Remove Session
                    }           
                    continue;
                }

                s.sendBytes += sendLen;
                if (s.recvBytes == s.sendBytes)
                {
                    s.recvBytes = 0;
                    s.sendBytes = 0;
                }

                cout << "Send Data = " << sendLen << '\n';
            }
        }

        // FD_CLOSE 처리
        if (networkEvents.lNetworkEvents & FD_CLOSE)
        {
            // TODO : Remove Socket
        }
    }

    ::WSACleanup();
}


