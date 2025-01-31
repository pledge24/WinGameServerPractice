#pragma once

#include <mutex>

template<typename T>
class LockStack
{
public:
    LockStack() {}

    LockStack(const LockStack&) = delete;               // 복사 생성자 막음
    LockStack& operator=(const LockStack&) = delete;    // 복사 연산자 막음

    void Push(T value)
    {
        lock_guard<mutex> lock(_mutex);
        _stack.push(std::move(value));
        _condVar.notify_one();
    }

    bool TryPop(T& value)
    {
        lock_guard<mutex> lock(_mutex);
        if (_stack.empty())
            return false;

        value = std::move(_stack.top());
        _stack.pop();
        return true;
    }

    void WaitPop(T& value)
    {
        unique_lock<mutex> lock(_mutex);
        _condVar.wait(lock, [this] { return _stack.empty() == false; });
        value = std::move(_stack.top());
        _stack.pop();
    }

private:
    stack<T> _stack;
    mutex _mutex;
    condition_variable _condVar;
};