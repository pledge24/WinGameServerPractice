#pragma once

#define OUT

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