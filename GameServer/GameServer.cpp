#include "pch.h"
#include <iostream>
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include "tchar.h"
#include "Protocol.pb.h"

int main(void)
{
    GSessionManager = new GameSessionManager();

    ClientPacketHandler::Init();

    ServerServiceRef service = MakeShared<ServerService>(
        NetAddress(L"127.0.0.1", 7777),
        MakeShared<IocpCore>(),
        MakeShared<GameSession>,    // TODO : SessoinManager 등
        100);

    ASSERT_CRASH(service->Start());

    for (int32 i = 0; i < 5; i++)
    {
        GThreadManager->Launch([=]()
            {
                while (true)
                {
                    service->GetIocpCore()->Dispatch();
                }
            });
    }

    while (true)
    {
        Protocol::S_TEST pkt;
        pkt.set_id(1000);
        pkt.set_hp(100);
        pkt.set_attack(10);
        {
            Protocol::BuffData* data = pkt.add_buffs();
            data->set_buffid(100);
            data->set_remaintime(1.2f);
            data->add_victims(4000);
        }
        {
            Protocol::BuffData* data = pkt.add_buffs();
            data->set_buffid(200);
            data->set_remaintime(2.5f);
            data->add_victims(1000);
            data->add_victims(2000);
        }
        
        SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
        GSessionManager->Broadcast(sendBuffer);

        this_thread::sleep_for(250ms);
    }

    GThreadManager->Join();
}

