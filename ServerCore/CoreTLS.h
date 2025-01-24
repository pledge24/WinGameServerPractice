#pragma once
#include <stack>

// 사용자 정의 쓰레드 id
extern thread_local uint32              LThreadId;
extern thread_local std::stack<int32>   LLockStack;