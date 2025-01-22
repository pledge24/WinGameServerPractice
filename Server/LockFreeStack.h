#pragma once

// -----------------
//      1차 시도
// -----------------

//struct SListEntry
//{
//    SListEntry* next;
//};
//
//class Data //: public SListEntry
//{
//public:
//    SListEntry _entry;
//
//    int32 _hp;
//    int32 _mp;
//};
//
//struct SListHeader
//{
//    SListEntry* next = nullptr;
//};
//
//// [data][ ][ ][ ]
//// [Header][next]
//
//void InitializeHead(SListHeader* header);
//void PushEntrySList(SListHeader* header, SListEntry* entry);
//SListEntry* PopEntrySList(SListHeader* header);

// -----------------
//      2차 시도
// -----------------

//struct SListEntry
//{
//    SListEntry* next;
//};
//
//class Data //: public SListEntry
//{
//public:
//    SListEntry _entry;
//
//    int32 _hp;
//    int32 _mp;
//};
//
//struct SListHeader
//{
//    SListEntry* next = nullptr;
//};
//
//void InitializeHead(SListHeader* header);
//void PushEntrySList(SListHeader* header, SListEntry* entry);
//SListEntry* PopEntrySList(SListHeader* header);

// -----------------
//      3차 시도
// -----------------

DECLSPEC_ALIGN(16)
struct SListEntry
{
    SListEntry* next;
};

DECLSPEC_ALIGN(16)
struct SListHeader
{
    union
    {
        struct
        {
            uint64 alignment;
            uint64 region;
        } DUMMYSTRUCTNAME;
        struct
        {
            uint64 depth : 16;
            uint64 sequence : 48;
            uint64 reserved : 4;
            uint64 next : 60;
        } HeaderX64;
    };
};

void InitializeHead(SListHeader* header);
void PushEntrySList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header);