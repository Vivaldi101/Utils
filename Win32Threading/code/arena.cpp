volatile byte* globalWin32VirtualMemoryBase;

// LIFO stack.
struct MemoryArena 
{
    byte *base;
    size_t maxByteCount;
    size_t usedByteCount;
};

function void * 
GetTopOfMemoryArena(MemoryArena *arena) 
{
    void *ptr = arena->base + arena->usedByteCount;
    return ptr;
}

// NOTE: dont use the _Push_ and _Pop_ functions directly, go through the macros
// FIXME: pass alignment
function void *
_Push_(MemoryArena *arena, size_t byteCount) 
{
    CheckMemory((arena->usedByteCount + byteCount) <= arena->maxByteCount);

    void *pushSlot = arena->base + arena->usedByteCount;
	byte* p = (byte*)ALIGNUP((byte*)pushSlot + byteCount - 1, GetWin32PageSize());

	// Make sure we can actually commit the reserved pages by going through the fault handler.
	// Write a zero byte at the last available slot to force the page fault.

    __try
	{
		*(p-1) = 0;
	}
	__except(PageFaultExceptionFilter(GetExceptionInformation()))
	{
		ExitWithFailure("PageFaultExceptionFilter failed\n");
	}

    CheckMemory(arena->usedByteCount <= arena->maxByteCount);

    arena->usedByteCount += byteCount;
    
    return pushSlot;
}

function void * 
_Pop_(MemoryArena *arena, size_t byteCount) 
{
    CheckMemory(arena->usedByteCount >= 0u);
    void *result;
    arena->usedByteCount -= byteCount;
    result = arena->base + arena->usedByteCount;
    
    CheckMemory(arena->usedByteCount >= 0u);

    return result;
}

function MemoryArena 
CreateMemoryArena(size_t byteCount) 
{
    MemoryArena result = {};

	result.base = (byte *)VirtualAlloc(0, byteCount, MEM_RESERVE, PAGE_NOACCESS);
    result.maxByteCount = byteCount;
    result.usedByteCount = 0;

	// Cleanup these.
	globalWin32VirtualMemoryBase = result.base;
    
    CheckMemory(result.base);
    CheckMemory(result.maxByteCount > 0);

    return result;
}

function size_t
IsMemoryArenaEmpty(MemoryArena *arena)
{
    return(!(arena->usedByteCount > 0));
}

function void
DestroyMemoryArena(MemoryArena *arena)
{
	assert(arena);
	assert(arena->base);

	// MEM_RELEASE does a MEM_DECOMMIT followed by MEM_RELEASE of the pages.

	VirtualFree(arena->base, 0, MEM_RELEASE);
	ZeroStruct(arena);

    CheckMemory(!arena->base);
    CheckMemory(arena->usedByteCount == 0);
    CheckMemory(arena->maxByteCount == 0);
}


static void *
_GetAt_(MemoryArena *ms, size_t size, u32 index)
{
    void *result = nullptr;
    result = ms->base + index*size;
    
    return result;
}


