#include <Windows.h>

class SyncQueue
{
private:
	int* queue;
	int size;
	int pushPos;
	int popPos;
	HANDLE toRead;
	HANDLE toWrite;
	CRITICAL_SECTION cs;

public:
	SyncQueue(int size)
	{
		this->pushPos = 0;
		this->popPos = 0;
		this->size = size;
		this->queue = new int[size];

		toRead = CreateSemaphore(NULL, 0, size, NULL);
		toWrite = CreateSemaphore(NULL, size, size, NULL);
		InitializeCriticalSection(&cs);
	}

	~SyncQueue()
	{
		DeleteCriticalSection(&cs);
		CloseHandle(toWrite);
		CloseHandle(toRead);
		delete[]queue;
	}

	void insert(int element)
	{
		WaitForSingleObject(toWrite, INFINITE);

		EnterCriticalSection(&cs);
		queue[pushPos++] = element;
		if (pushPos == size)
		{
			pushPos = 0;
		}
		LeaveCriticalSection(&cs);

		ReleaseSemaphore(toRead, 1, NULL);
	}

	int remove()
	{
		WaitForSingleObject(toRead, INFINITE);

		EnterCriticalSection(&cs);
		int element = queue[popPos++];
		if (popPos == size)
		{
			popPos = 0;
		}
		LeaveCriticalSection(&cs);

		ReleaseSemaphore(toWrite, 1, NULL);

		return element;
	}
};