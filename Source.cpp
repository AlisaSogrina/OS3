#include <iostream>
#include <ctime>
#include "SyncQueue.cpp"

using namespace std;

HANDLE start;

struct DataForProducer
{
	SyncQueue* queue;
	int number;
	int count;
};

struct DataForConsumer
{
	SyncQueue* queue;
	int count;
};

DWORD WINAPI producer(LPVOID par)
{
	WaitForSingleObject(start, INFINITE);

	DataForProducer* data = (DataForProducer*)par;


	for (int i = 0; i < data->count; i++)
	{
		data->queue->insert(data->number * 100 + i);
		cout << "Thread " << data->number << " put in queue number :\"" << data->number * 100 + i << "\"\n";
		Sleep(17);
	}

	return 0;
}

DWORD WINAPI consumer(LPVOID par)
{
	WaitForSingleObject(start, INFINITE);

	DataForConsumer* data = (DataForConsumer*)par;

	for (int i = 0; i < data->count; i++)
	{
		int number = data->queue->remove();
		cout << "\t\tNumber \"" << number << "\" was removed from queue.\n";
		Sleep(17);
	}

	return 0;
}

void main()
{
	cout << "Enter queue size:\n";
	int queueSize;
	cin >> queueSize;

	start = CreateEvent(NULL, TRUE, FALSE, NULL);

	SyncQueue* queue = new SyncQueue(queueSize);

	cout << "Enter Producer count:\n";
	int prodThreadCount;
	cin >> prodThreadCount;

	DataForProducer* workWithProductCountProducer = new DataForProducer[prodThreadCount];
	for (int i = 0; i < prodThreadCount; i++)
	{
		cout << "Enter number of produce in " << i + 1 << " thread:\n";
		int count;
		cin >> count;
		workWithProductCountProducer[i].count = count;
		workWithProductCountProducer[i].number = i + 1;
		workWithProductCountProducer[i].queue = *&queue;
	}

	HANDLE* prodThreadHandls = new HANDLE[prodThreadCount];
	DWORD* prodThreadID = new DWORD[prodThreadCount];

	for (int i = 0; i < prodThreadCount; i++)
	{
		prodThreadHandls[i] = CreateThread(NULL, 0, producer, (void*)&workWithProductCountProducer[i], 0, &prodThreadID[i]);
	}


	cout << "Enter Consumer count:\n";
	int consThreadCount;
	cin >> consThreadCount;

	DataForConsumer* workWithProductCountConsumer = new DataForConsumer[consThreadCount];
	for (int i = 0; i < consThreadCount; i++)
	{
		cout << "Enter number of cunsumer products in " << i + 1 << " thread:\n";
		int count;
		cin >> count;
		workWithProductCountConsumer[i].count = count;
		workWithProductCountConsumer[i].queue = *&queue;
	}

	HANDLE* consThreadHandls = new HANDLE[consThreadCount];
	DWORD* consThreadID = new DWORD[consThreadCount];

	for (int i = 0; i < consThreadCount; i++)
	{
		consThreadHandls[i] = CreateThread(NULL, 0, consumer, (void*)&workWithProductCountConsumer[i], 0, &consThreadID[i]);
	}

	HANDLE* handlesArray = new HANDLE[prodThreadCount + consThreadCount];

	for (int i = 0; i < prodThreadCount; i++)
	{
		handlesArray[i] = prodThreadHandls[i];
	}

	for (int i = 0; i < consThreadCount; i++)
	{
		handlesArray[i + prodThreadCount] = consThreadHandls[i];
	}

	SetEvent(start);

	WaitForMultipleObjects(prodThreadCount + consThreadCount, handlesArray, TRUE, INFINITE);
	system("pause");
}