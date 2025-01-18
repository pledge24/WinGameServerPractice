#pragma once

#include <mutex>

template<typename T>
class LockQueue
{
public: 
	LockQueue() { }

	LockQueue(const LockQueue&) = delete;               // 복사 생성자 막음
	LockQueue& operator=(const LockQueue&) = delete;    // 복사 연산자 막음

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_queue.push(std::move(value));
		_condVar.notify_one();
	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_queue.empty())
			return false;

		value = std::move(_queue.front());
		_queue.pop();
		return true;
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] { return _queue.empty() == false; });
		value = std::move(_queue.front());
		_queue.pop();
	}

private:
	queue<T> _queue;
	mutex _mutex;
	condition_variable _condVar;
};