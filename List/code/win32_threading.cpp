#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "common.cpp"
#include "win32_common.cpp"

#include "arena.cpp"

struct ListHead
{
    ListHead* previous;
    ListHead* next;
};

struct Task
{
    int pid; 
    ListHead tasks;
};

struct ThreadState
{
	Task* tasks;
	CRITICAL_SECTION* csection;
	int jobCount;
};

static void ListDestroy(ListHead* head)
{
	for (Task* cursor = ListEntry((head)->next, Task, tasks);
		&(cursor)->tasks != head;)
	{
		Task* next = ListEntry(cursor->tasks.next, Task, tasks);
		cursor = next;
	}
}

static void InsertInBetween(ListHead* node, ListHead* previous, ListHead* next)
{
	next->previous = node;
	node->next = next;
	node->previous = previous;
	previous->next = node;
}

static void PushToList(ListHead* node, ListHead* head)
{
	assert(head);
	assert(head->next);

	InsertInBetween(node, head, head->next);
}

static void TryTestNewPIDs(int jobCount, ListHead* taskHead, Task* const tasks)
{
	for (int i = 0; i < jobCount; ++i)
	{
		Task* tp = tasks + i;

		if (!tp)
			continue;

		tp->pid = i;

		PushToList(&tp->tasks, taskHead);
	}
}

DWORD WINAPI 
WorkerThread(LPVOID threadParameter)
{
	ThreadState* ts = (ThreadState*)threadParameter;

    ListHead taskHead = {};
	ListCreate(&taskHead);

	const int jobCount = ts->jobCount;
	Task* tasks = ts->tasks;

	// Do some work for the tasks.
	TryTestNewPIDs(jobCount, &taskHead, tasks);

	return 0;
}

int 
main()
{
	const DWORD coreCount = GetWin32CoreCount();
	const unsigned int TASK_COUNT = coreCount * 10;

	HANDLE* threadPool = (HANDLE*)alloca(coreCount * sizeof(HANDLE));

	ThreadState* threadStates = (ThreadState*)alloca(coreCount * sizeof(ThreadState));

	CRITICAL_SECTION csection;
	InitializeCriticalSection(&csection); 

	unsigned int freeThreadIndex = 0u;
	int taskCount = TASK_COUNT;
	const int jobCount = 3;

	MemoryArena mainArena = CreateMemoryArena((1ll << 46ll));
	Task* tasks = 0;

	while (taskCount > 0)
	{
		// Have a free thread for a task.
		if (freeThreadIndex < coreCount)
		{
			tasks = PushArray(&mainArena, jobCount, Task);

			if (!tasks)
				break;

			threadStates[freeThreadIndex].tasks = tasks;
			threadStates[freeThreadIndex].csection = &csection;
			threadStates[freeThreadIndex].jobCount = jobCount;

			DWORD threadID;
			if (!(threadPool[freeThreadIndex] = CreateThread(NULL, 0, WorkerThread, &threadStates[freeThreadIndex], 0, &threadID)))
				abort();

			++freeThreadIndex;
			--taskCount;

			assert(freeThreadIndex <= coreCount);

			continue;
		}

		// At max.
		assert(freeThreadIndex == coreCount);

		// Wait for the first to complete.
		freeThreadIndex = WaitForMultipleObjects(coreCount, threadPool, FALSE, INFINITE) - WAIT_OBJECT_0;

		assert(freeThreadIndex < coreCount);
	}

	WaitForMultipleObjects(coreCount < TASK_COUNT ? coreCount : TASK_COUNT, threadPool, TRUE, INFINITE);

	for (unsigned int i = 0; i < coreCount; ++i)
		CloseHandle(threadPool[i]);

	DeleteCriticalSection(&csection);

	PopArray(&mainArena, jobCount*TASK_COUNT, Task);

	DestroyMemoryArena(&mainArena);

	return 0;
}
