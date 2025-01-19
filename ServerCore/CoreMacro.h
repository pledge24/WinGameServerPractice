#pragma once

#define OUT

/*-----------------
       Lock
------------------*/

#define USE_MANY_LOCKS(count)       Lock _locks[count];
#define USE_LOCK                    USE_MANY_LOCKS(1)
#define READ_LOCK_IDX(idx)          ReadLockGuard readLockGuard_##idx(_locks[idx]);
#define READ_LOCK                   READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx)         WriteLockGuard writeLockGuard_##idx(_locks[idx]);
#define WRITE_LOCK                  WRITE_LOCK_IDX(0)

/*-----------------
       Crash
------------------*/

// 인위적 크래쉬 발생
#define CRASH(cause)                        \
{                                           \
    uint32* crash = nullptr;                \
    __analysis_assume(crash != nullptr);    \
    *crash = 0xDEADBEEF;                    \
}          

// 조건부 크래쉬 발생
#define ASSERT_CRASH(expr)                  \
{                                           \
    if (!(expr))                            \
    {                                       \
        CRASH("ASSERT_CRASH");              \
        __analysis_assume(expr);            \
    }                                       \
}                                           