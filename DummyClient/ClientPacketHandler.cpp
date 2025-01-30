#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"

void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
    BufferReader br(buffer, len);

    PacketHeader header;
    br >> header;

    switch (header.id)
    {
    case S_TEST:
        Handle_S_TEST(buffer, len);
        break;

    }
}

#pragma pack(1)
// 패킷 설계 TEMP
struct BuffData
{
    uint64 buffId;
    float remainTime;
};

// [PKT_S_TEST][BuffsListItem BuffsListItem BuffsListItem]
struct PKT_S_TEST
{
    struct BuffsListItem
    {
        uint64 buffId;
        float remainTime;
    };

    uint16 packetSize;  // 공용 헤더
    uint16 packetId;    // 공용 헤더
    uint64 id;          // 8바이트
    uint32 hp;          // 4바이트
    uint16 attack;      // 2바이트
    uint16 buffsOffset; // 가변 길이 오프셋
    uint16 buffsCount;  // 버프 개수

    bool Validate()
    {
        uint32 size = 0;
        size += sizeof(PKT_S_TEST);
        if (packetSize < size)
            return false;

        size += buffsCount * sizeof(BuffsListItem);
        if (size != packetSize)
            return false;

        if (buffsOffset + buffsCount * sizeof(BuffsListItem) < packetSize)
            return false;

        return true;
    }

    using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;

    BuffsList GetBuffsList()
    {
        BYTE* data = reinterpret_cast<BYTE*>(this);
        data += buffsOffset;
        return BuffsList(reinterpret_cast<PKT_S_TEST::BuffsListItem*>(data), buffsCount);
    }
};
#pragma pack()

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
    BufferReader br(buffer, len);

    if (len < sizeof(PKT_S_TEST))
        return;

    PKT_S_TEST* pkt = reinterpret_cast<PKT_S_TEST*>(buffer);

    if (pkt->Validate() == false)
        return;

    //cout << "ID: " << id << " HP : " << hp << " ATT : " << attack << endl;

    PKT_S_TEST::BuffsList buffs = pkt->GetBuffsList();

    cout << "BuffCount : " << buffs.Count() << endl;
    for (int32 i = 0; i < pkt->buffsCount; i++)
    {
        cout << "BuffInfo: " << buffs[i].buffId << " " << buffs[i].remainTime << endl;
    }

    for (auto it = buffs.begin(); it != buffs.end(); ++it)
    {
        cout << "BuffInfo: " << it->buffId << " " << it->remainTime << endl;
    }

    for (auto& buff : buffs)
    {
        cout << "BuffInfo: " << buff.buffId << " " << buff.remainTime << endl;

    }
}

