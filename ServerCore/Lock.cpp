#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
#include "DeadLockProfiler.h"

void Lock::WriteLock(const char* name)
{

#if _DEBUG
    GDeadLockProfiler->PushLock(name);
#endif
    // 동일한 쓰레드가 소유하고 있다면 무조건 성공.
    const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
    if (LThreadId == lockThreadId)
    {
        _writeCount++;
        return;
    }

    // 아무도 소유 및 공유하고 있지않을 때, 경합해서 소유권을 얻는다.
    const int64 beginTick = ::GetTickCount64();
    const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
    while (true)
    {
        for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
        {
            uint32 expected = EMPTY_FLAG;
            if (_lockFlag.compare_exchange_strong(OUT expected, desired))
            {
                _writeCount++;
                return;
            }
        }

        if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
            CRASH("LOCK_TIMEOUT");
        
        this_thread::yield();
    }
}

void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
    GDeadLockProfiler->PopLock(name);
#endif
    // ReadLock을 다 풀기 전에는 WriteUnlock 불가능(버그잡기용)
    if ((_lockFlag.load() & READ_COUNT_MASK) != 0)
        CRASH("INVALID_UNLOCK_ORDER");

    const int32 lockCount = --_writeCount;
    if (lockCount == 0)
        _lockFlag.store(EMPTY_FLAG);
}

void Lock::ReadLock(const char* name)
{
#if _DEBUG
    GDeadLockProfiler->PushLock(name);
#endif
    // 동일한 쓰레드가 소유하고 있다면 무조건 성공.(어차피 다른애들은 접근 못하는 상태)
    const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
    if (LThreadId == lockThreadId)
    {
        _lockFlag.fetch_add(1);
        return;
    }

    // 아무도 소유하고 있지 않을 때 경합해서 공유 카운트를 올린다.
    const int64 beginTick = ::GetTickCount64();
    while (true)
    {
        for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
        {
            uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);
            if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
                return;
        }

        if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
            CRASH("LOCK_TIMEOUT");

        this_thread::yield();
    }
}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
    GDeadLockProfiler->PopLock(name);
#endif
    // 크래시가 일어날 일은 거의 없을것임
    if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
        CRASH("MULTIPLE_UNLOCK");
}
