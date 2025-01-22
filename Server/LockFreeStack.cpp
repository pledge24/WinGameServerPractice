#include "pch.h"
#include "LockFreeStack.h"

// -----------------
//      1차 시도
// -----------------

//void InitializeHead(SListHeader* header)
//{
//    header->next = nullptr;
//}
//
//void PushEntrySList(SListHeader* header, SListEntry* entry)
//{
//    entry->next = header->next;
//    header->next = entry;
//}
//
//SListEntry* PopEntrySList(SListHeader* header)
//{
//    SListEntry* first = header->next;
//
//    if (first != nullptr)
//        header->next = first->next;
//
//    return first;
//}

// -----------------
//      2차 시도
// -----------------

//void InitializeHead(SListHeader* header)
//{
//    header->next = nullptr;
//}
//
//void PushEntrySList(SListHeader* header, SListEntry* entry)
//{
//    entry->next = header->next;
//    while (::InterlockedCompareExchange64((int64*)&header->next, (int64)entry, (int64)entry->next) == 0)
//    {
//
//    }
//}
//
//SListEntry* PopEntrySList(SListHeader* header)
//{
//    SListEntry* expected = header->next;
//
//    //ABA Problem
//    // 
//    //[5000] -> [7000]
//    // 
//    //만약에 Header가 5000이라면, Header에다가 6000을 넣어줘!
//    //[5000]->[6000]->[7000]
//    // [Header]
//    while (expected && ::InterlockedCompareExchange64((int64*)&header->next, (int64)expected->next, (int64)expected) == 0)
//    {
//
//    }
//
//    return expected;
//}

// -----------------
//      3차 시도
// -----------------

void InitializeHead(SListHeader* header)
{
    header->alignment = 0;
    header->region = 0;
}

void PushEntrySList(SListHeader* header, SListEntry* entry)
{
    SListHeader expected = {};
    SListHeader desired = {};

    // 16 바이트 정렬
    desired.HeaderX64.next = (((uint64)entry) >> 4);

    while (true)
    {
        expected = *header;

        // 이 사이에 변경될 수 있다.

        entry->next = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);
        desired.HeaderX64.depth = expected.HeaderX64.depth + 1;
        desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

        if (::InterlockedCompareExchange128((int64*)header, desired.region, desired.alignment, (int64*)&expected) == 1)
            break;
    }
}

SListEntry* PopEntrySList(SListHeader* header)
{
    SListHeader expected = {};
    SListHeader desired = {};
    SListEntry* entry = nullptr;

    while (true)
    {
        expected = *header;

        entry = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);
        if (entry == nullptr)
            break;

        // Use-After-Free
        desired.HeaderX64.next = ((uint64)entry->next) >> 4;
        desired.HeaderX64.depth = expected.HeaderX64.depth - 1;
        desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

        if (::InterlockedCompareExchange128((int64*)header, desired.region, desired.alignment, (int64*)&expected) == 1)
            break;
    }

    return entry;
}