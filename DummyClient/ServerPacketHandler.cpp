#include "pch.h"
#include "ServerPacketHandler.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// 직접 컨텐츠 작업자

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
    // TODO: Log
    return false;
}

bool Handle_S_TEST(PacketSessionRef& session, Protocol::S_TEST& pkt)
{
    cout << pkt.id() << " " << pkt.hp() << " " << pkt.attack() << endl;

    cout << "BUFSIZE : " << pkt.buffs_size() << endl;

    for (auto& buf : pkt.buffs())
    {
        cout << "BUFINFO : " << buf.buffid() << " " << buf.remaintime() << endl;
        cout << "VICTIMS : " << buf.victims_size() << endl;
        for (auto& vic : buf.victims())
        {
            cout << vic << " ";
        }

        cout << endl;
    }

    return true;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
    return true;
}

