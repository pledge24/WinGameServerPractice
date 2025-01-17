#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

#include <thread>

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "Memory.h"

void HandleError(const char* cause)
{
    int32 errCode = ::WSAGetLastError();
    cout << cause << " ErrorCode : " << errCode << endl;
}

const int32 BUFSIZE = 1000;

struct Session
{
    SOCKET socket = INVALID_SOCKET;
    char recvBuffer[BUFSIZE] = {};
    int32 recvBytes = 0;
};

enum IO_TYPE
{
    READ,
    WRITE,
    ACCEPT,
    CONNECT,
};

struct OverlappedEx
{
    WSAOVERLAPPED overlapped = {};
    int32 type = 0; // read, write, accept, connect ...
};

void WorkerThreadMain(HANDLE iocpHandle)
{
    while (true)
    {
        DWORD bytesTransferred = 0;
        Session* session = nullptr;
        OverlappedEx* overlappedEx = nullptr;

        BOOL ret = ::GetQueuedCompletionStatus(iocpHandle, &bytesTransferred,
            (ULONG_PTR*)&session, (LPOVERLAPPED*)&overlappedEx, INFINITE);

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

int64 result;

int64 Calculate()
{
    int64 sum = 0;

    for (int32 i = 0; i < 100'000; i++)
        sum += i;

    result = sum;

    return sum;
}

void PromiseWorker(std::promise<string>&& promise)
{
    promise.set_value("Secret Message");
}

void TaskWorker(std::packaged_task<int64(void)>&& task)
{
    task();
}

int main()
{
    // 동기(synchronous) 실행
    // int64 sum = Calculate();
    // cout << sum << '\n';

    // std::future
    {
        // 1) deferred: lazy evaluation 지연해서 실행
        // 2) async: 별도의 쓰레드를 만들어서 실행
        // 3) deferred | async: 둘 중 알아서
        // 언젠가 미래에 결과를 뱉어준다!
        std::future<int64> future = std::async(std::launch::async, Calculate);

        // TODO

        // 작업의 진척도를 보는 peek 기능
        std::future_status status =  future.wait_for(1ms);
        //if (status == future_status::ready)
        //{
        //    // 처리
        //}

        // 생략하고 get을 호출해도 결과는 같다.
        //future.wait(); // wait_for(INFINITE) 

        int64 sum = future.get(); // 결과물이 이제서야 필요하다!

        // 클래스 멤버 함수를 넣어주고 싶다면?
        class Knight
        {
        public:
            int64 GetHp() { return 100; }
        };

        Knight knight;
        std::future<int64> future2 = std::async(std::launch::async, &Knight::GetHp, knight);
    }

    // std::promise
    {
        // 미래(std::future)에 결과물을 반환해줄거라 약속(std::promise)해줘~ (계약서?)
        std::promise<string> promise;
        std::future<string> future = promise.get_future();

        thread t(PromiseWorker, std::move(promise));

        string message = future.get();
        cout << message << endl;

        t.join();
    }

    // std::packaged_Task
    {
        std::packaged_task<int64(void)> task(Calculate);
        std::future<int64> future = task.get_future();

        std::thread t(TaskWorker, std::move(task));

        int64 sum = future.get();
        cout << sum << endl;

        t.join();
    }
 
}