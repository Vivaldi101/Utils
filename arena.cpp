
// LIFO stack.
struct MemoryArena 
{
    byte *	base;
    size_t	maxByteCount;
    size_t	usedByteCount;
};

#define GetArrayElements(arena, type) ((type *)arena.base)
#define GetAt(arena, type, index) ((type *)_GetAt_(arena, sizeof(type), index))
#define GetLast(arena, type) ((type *)_GetLastElement_(arena, sizeof(type)))
#define PushStruct(arena, type) ((type *)_Push_(arena, sizeof(type)))  

#define PushSize(arena, size, type) ((type *)_Push_(arena, size))  
#define PopStruct(arena, type) ((type *)_Pop_(arena, sizeof(type)))  

#define PushArray(arena, count, type) ((type *)_Push_(arena, (count) * sizeof(type)))
#define PopArray(arena, count, type) ((type *)_Pop_(arena, (count) * sizeof(type)))
#define CheckMemory(cond) do { if (!(cond)) { MessageBoxA(0, "Out of memory in: " ##__FILE__, 0, 0); DebugBreak(); } } while(0)
#define PrintMessageBox(msg) MessageBoxA(0, msg, 0, 0)


static void * 
GetTopOfMemoryArena(MemoryArena *arena) 
{
    void *ptr = arena->base + arena->usedByteCount;
    return ptr;
}

// NOTE: dont use the _Push_ and _Pop_ functions directly, go through the macros
// FIXME: pass alignment
static void *
_Push_(MemoryArena *arena, size_t byteCount) 
{
    CheckMemory((arena->usedByteCount + byteCount) <= arena->maxByteCount);

    void *result = arena->base + arena->usedByteCount;
    arena->usedByteCount += byteCount;

    CheckMemory(arena->usedByteCount <= arena->maxByteCount);
    
    return result;
}

static void * 
_Pop_(MemoryArena *arena, size_t byteCount) 
{
    CheckMemory(arena->usedByteCount >= 0u);
    void *result;
    arena->usedByteCount -= byteCount;
    result = arena->base + arena->usedByteCount;
    
    CheckMemory(arena->usedByteCount >= 0u);
    return result;
}

static MemoryArena 
CreateMemoryArena(size_t byteCount) 
{
    MemoryArena result = {};
	// TODO: Page fault hander and commit later.
    void *ptr = VirtualAlloc(0, byteCount, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    
    result.base = (byte *)ptr;
    result.maxByteCount = byteCount;
    result.usedByteCount = 0;
    
    CheckMemory(result.base);
    return result;
}

static size_t
IsMemoryArenaEmpty(MemoryArena *arena)
{
    return(!(arena->usedByteCount > 0));
}

static void
DestroyMemoryArena(MemoryArena *arena)
{
    if(arena->base)
    {
        VirtualFree(arena->base, 0, MEM_RELEASE);
        ZeroStruct(arena);
    }
}
