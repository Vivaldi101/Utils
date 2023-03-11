#include "wad.h"

typedef struct Link
{
	struct Link* next;
	WadDirectory wadDirectory;
} Link;

typedef struct List
{
	Link* first;
	Link* last;
} List;

typedef struct LinkPool
{
	Link* firstFree;
	MemoryArena arena;	// Arena for the freelist.
	u32 linkCount;
} LinkPool;

function void
AppendToList(List *list, Link *link)
{
	list->last = (list->last ? list->last->next : list->first) = link;
}

function void*
AllocateFromHeap(size_t byteCount)
{
	void *result = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, byteCount);
	return result;
}

function void
FreeBackToHeap(void *memory)
{
	HeapFree(GetProcessHeap(), 0, memory);
}

function Link*
GetFirstFreePoolLink(LinkPool* pool)
{
	Link *result = 0;

	result = pool->firstFree;
	pool->firstFree = pool->firstFree->next;

	result->next = 0;
	ZeroStruct(&result->wadDirectory); // Remove this. Just here to test the wad stuff.

	assert(result);

	return result;
}

function Link*
AllocateLink(LinkPool* pool)
{
	if (pool->firstFree)
	{
		return GetFirstFreePoolLink(pool);
	}

	u32 linkCountInPool = pool->linkCount;

	pool->linkCount = linkCountInPool;

	Link* newLinks = PushArray(&pool->arena, linkCountInPool, Link);

	if (!newLinks)
	{
		ExitWithFailure(__FUNCTION__);
	}

	for (u32 linkIndex = 0; 
		linkIndex < (linkCountInPool - 1);
		++linkIndex)
	{
		// Chain the links.
		newLinks[linkIndex].next = &newLinks[linkIndex+1];
	}

	// Set the last link's next in the pool to null to mark the end.
	newLinks[linkCountInPool-1].next = 0;

	pool->firstFree = newLinks;

	return GetFirstFreePoolLink(pool);
}

function void 
ReturnLinkToPool(Link *link, LinkPool *pool)
{
	ZeroStruct(&link->wadDirectory); // Remove this. Just here to test the wad stuff.
	link->next = pool->firstFree;
	pool->firstFree = link;
}

