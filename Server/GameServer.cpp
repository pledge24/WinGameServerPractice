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
};

enum IO_TYPE
{
    READ,
    WRITE,
    ACCEPT,
    CONNECT
};

struct OverlappedEx
{
    WSAOVERLAPPED overlapped = {};
    int32 type = 0; // read, write, accept, connect
};

void WorkerThreadMain(HANDLE iocpHandle)
{
    while (true)
    {
        DWORD bytesTransferred = 0;
        Session* session = nullptr;
        OverlappedEx* overlappedEx = nullptr;

        BOOL ret = ::GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, /*Key*/(ULONG_PTR*)&session, (LPOVERLAPPED*)&overlappedEx, INFINITE);

        if (ret == FALSE || bytesTransferred == 0)
        {
            // TODO : 연결 끊김
            continue;
        }

        ASSERT_CRASH(overlappedEx->type == IO_TYPE::READ);

        cout << "Recv Data IOCP = " << bytesTransferred << endl;

        WSABUF wsaBuf;
        wsaBuf.buf = session->recvBuffer;
        wsaBuf.len = BUFSIZE;

        DWORD recvLen = 0;
        DWORD flags = 0;
        ::WSARecv(session->socket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
    }

}

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

    if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
        return 0;

    cout << "Accept" << '\n';

    vector<Session*> sessionManager;

    // CP 생성
    HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    // WorkerThreads
    for (int32 i = 0; i < 5; i++)
    {
        GThreadManager->Launch([=]() { WorkerThreadMain(iocpHandle); });
    }

    // Main Thread = Accept 담당
    while (true)
    {
        SOCKADDR_IN clientAddr;
        int32 addrLen = sizeof(clientAddr);

        SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);;
        if (clientSocket == INVALID_SOCKET)
            return 0;
        
        Session* session = new Session();
        session->socket = clientSocket;

        cout << "Client Connected!" << endl;

        // 소켓을 CP에 등록
        ::CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, /*Key*/(ULONG_PTR)session, 0);

        WSABUF wsaBuf;
        wsaBuf.buf = session->recvBuffer;
        wsaBuf.len = BUFSIZE;

        OverlappedEx* overlappedEx = new OverlappedEx();
        overlappedEx->type = IO_TYPE::READ;

        DWORD recvLen = 0;
        DWORD flags = 0;
        ::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
    }

    GThreadManager->Join();

    ::WSACleanup();
}


