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
        size += buffsCount * sizeof(BuffsListItem);
        if (size != packetSize)
            return false;

        if (buffsOffset + buffsCount * sizeof(BuffsListItem) < packetSize)
            return false;

        return true;
    }
};
#pragma pack()

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
    BufferReader br(buffer, len);

    if (len < sizeof(PKT_S_TEST))
        return;

    PKT_S_TEST pkt;
    br >> pkt;

    if (pkt.Validate() == false)
        return;

    //cout << "ID: " << id << " HP : " << hp << " ATT : " << attack << endl;

    vector<PKT_S_TEST::BuffsListItem> buffs;

    buffs.resize(pkt.buffsCount);
    for (int32 i = 0; i < pkt.buffsCount; i++)
    {
        br >> buffs[i];
    }

    cout << "BuffCount : " << pkt.buffsCount << endl;
    for (int32 i = 0; i < pkt.buffsCount; i++)
    {
        cout << "BuffInfo: " << buffs[i].buffId << " " << buffs[i].remainTime << endl;
    }

    wstring name;
    uint16 nameLen;
    br >> nameLen;
    name.resize(nameLen);

    br.Read((void*)name.data(), nameLen * sizeof(WCHAR));

    wcout.imbue(std::locale("kor"));

    wcout << name << endl;
}
