#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "CoreMacro.h"
#include "ThreadManager.h"

#include "RefCounting.h"
#include "Memory.h"
#include "ObjectPool.h"

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")



//void HandleError(const char* cause)
//{
//    int32 errCode = ::WSAGetLastError();
//    cout << cause << " ErrorCode : " << errCode << endl;
//}
//
//const int32 BUFSIZE = 1000;
//
//struct Session
//{
//    SOCKET socket = INVALID_SOCKET;
//    char recvBuffer[BUFSIZE] = {};
//    int32 recvBytes = 0;
//};
//
//enum IO_TYPE
//{
//    READ,
//    WRITE,
//    ACCEPT,
//    CONNECT,
//};
//
//struct OverlappedEx
//{
//    WSAOVERLAPPED overlapped = {};
//    int32 type = 0; // read, write, accept, connect ...
//};
//
//void WorkerThreadMain(HANDLE iocpHandle)
//{
//    while (true)
//    {
//        DWORD bytesTransferred = 0;
//        Session* session = nullptr;
//        OverlappedEx* overlappedEx = nullptr;
//
//        BOOL ret = ::GetQueuedCompletionStatus(iocpHandle, &bytesTransferred,
//            (ULONG_PTR*)&session, (LPOVERLAPPED*)&overlappedEx, INFINITE);
//
//        if (ret == FALSE || bytesTransferred == 0)
//        {
//            // TODO : 연결 끊김
//            continue;
//        }
//
//        ASSERT_CRASH(overlappedEx->type == IO_TYPE::READ);
//
//        cout << "Recv Data IOCP = " << bytesTransferred << endl;
//
//        WSABUF wsaBuf;
//        wsaBuf.buf = session->recvBuffer;
//        wsaBuf.len = BUFSIZE;
//
//        DWORD recvLen = 0;
//        DWORD flags = 0;
//        ::WSARecv(session->socket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
//    }
//}

//int main()
//{
//    WSAData wsaData;
//    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//        return 0;
//
//    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
//    if (listenSocket == INVALID_SOCKET)
//        return 0;
//
//    SOCKADDR_IN serverAddr;
//    ::memset(&serverAddr, 0, sizeof(serverAddr));
//    serverAddr.sin_family = AF_INET;
//    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
//    serverAddr.sin_port = ::htons(7777);
//
//    if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
//        return 0;
//
//    if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
//        return 0;
//
//    cout << "Accept" << endl;
//
//    // Overlapped 모델 (Completion Routine 콜백 기반)
//    // - 비동기 입출력 함수 완료되면, 쓰레드마다 있는 APC 큐에 일감이 쌓임
//    // - Alertable Wait 상태로 들어가서 APC 큐 비우기 (콜백 함수)
//    // 단점) APC큐 쓰레드마다 있다! Alertable Wait 자체도 조금 부담!
//    // 단점) 이벤트 방식 소켓:이벤트 1:1 대응
//
//    // IOCP (Completion Port) 모델
//    // - APC -> Completion Port (쓰레드마다 있는건 아니고 1개. 중앙에서 관리하는 APC 큐?)
//    // - Alertable Wait -> CP 결과 처리를 GetQueuedCompletionStatus
//    // 쓰레드랑 궁합이 굉장히 좋다!
//
//    // CreateIoCompletionPort
//    // GetQueuedCompletionStatus
//
//    vector<Session*> sessionManager;
//
//    // CP 생성
//    HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
//
//    // WorkerThreads
//    for (int32 i = 0; i < 5; i++)
//        GThreadManager->Launch([=]() { WorkerThreadMain(iocpHandle); });
//
//    // Main Thread = Accept 담당
//    while (true)
//    {
//        SOCKADDR_IN clientAddr;
//        int32 addrLen = sizeof(clientAddr);
//
//        SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
//        if (clientSocket == INVALID_SOCKET)
//            return 0;
//
//        Session* session = new Session();
//        session->socket = clientSocket;
//        sessionManager.push_back(session);
//
//        cout << "Client Connected !" << endl;
//
//        // 소켓을 CP에 등록
//        ::CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, /*Key*/(ULONG_PTR)session, 0);
//
//        WSABUF wsaBuf;
//        wsaBuf.buf = session->recvBuffer;
//        wsaBuf.len = BUFSIZE;
//
//        OverlappedEx* overlappedEx = new OverlappedEx();
//        overlappedEx->type = IO_TYPE::READ;
//
//        // ADD_REF
//        DWORD recvLen = 0;
//        DWORD flags = 0;
//        ::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
//
//        // 유저가 게임 접속 종료!
//        //Session* s = sessionManager.back();
//        //sessionManager.pop_back();
//        //xdelete(s);
//
//        //::closesocket(session.socket);
//        //::WSACloseEvent(wsaEvent);
//    }
//
//    GThreadManager->Join();
//
//    // 윈속 종료
//    ::WSACleanup();
//}

#include "TypeCast.h"

using TL = TypeList<class Player, class Mage, class Knight, class Archer>;

class Player
{
    
public:
    Player()
    {
        INIT_TL(Player);
    }
    virtual ~Player() {}

    DECLARE_TL
};

class Knight : public Player
{
public:
    Knight() { INIT_TL(Knight) }
};

class Mage : public Player
{
public:
    Mage() { INIT_TL(Mage) }
};

class Archer : public Player
{
public:
    Archer() { INIT_TL(Archer) }
};

class Dog
{

};

#include "TypeCast.h"

int main(void)
{
    //TypeList<Mage, Knight>::Head whoAmI;
    //TypeList<Mage, Knight>::Tail whoAmI2;

    //TypeList<Mage, TypeList<Knight, Archer>>::Head whoAMI3;
    //TypeList<Mage, TypeList<Knight, Archer>>::Tail::Head whoAMI4;
    //TypeList<Mage, TypeList<Knight, Archer>>::Tail::Tail whoAMI5;

    //int32 len1 = Length<TypeList<Mage, Knight>>::value;
    //int32 len2 = Length<TypeList<Mage, Knight, Archer>>::value;

    //TypeAt<TL, 0>::Result whoAMI6;
    //TypeAt<TL, 1>::Result whoAMI7;
    //TypeAt<TL, 2>::Result whoAMI8;

    //int32 index1 = IndexOf<TL, Mage>::value;
    //int32 index2 = IndexOf<TL, Archer>::value;
    //int32 index3 = IndexOf<TL, Dog>::value;

    //bool canConvert1 = Conversion<Player, Knight>::exists;   // 0
    //bool canConvert2 = Conversion<Knight, Player>::exists;   // 1
    //bool canConvert3 = Conversion<Knight, Dog>::exists;      // 0

    /*{
        Player* player = new Knight();

        bool canCast = CanCast<Knight*>(player);
        Knight* knight = TypeCast<Knight*>(player);

        delete player;
    }*/

    {
        shared_ptr<Player> player = MakeShared<Knight>();

        shared_ptr<Archer> archer = TypeCast<Archer>(player);
        bool canCast = CanCast<Mage>(player);
    }

    for (int32 i = 0; i < 5; i++)
    {
        GThreadManager->Launch([]()
            {
                while (true)
                {
                }
            });
    }

    GThreadManager->Join();
}