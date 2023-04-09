#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdint.h>
#include <assert.h>
#include "list.h"

typedef unsigned char byte;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef u32 b32;

#define function static
#define global static
#define local static

#define MakeString(s) {sizeof(s)-1, s}
#define ExitWithFailure(...) printf("Failed here: " __VA_ARGS__); __debugbreak(); ExitProcess(GetLastError())

#define Cast(x, t) (t)(x)
#define ZeroStruct(x) memset((x), 0, sizeof(*(x)));

#define Implies(a, b) assert(!(a) || (b))
#define PreCondition(a) assert((a))
#define PostCondition(a) assert((a))

#define Defer(begin, end) int (_i_); for((_i_) = (begin, 0); !(_i_); ++(_i_), end)
#define Ensure(begin, end) int (_i_); assert(begin); for((_i_) = ((begin), 0); !(_i_); ++(_i_), assert(end))

#define GetArrayElements(arena, type) ((type *)arena.base)
#define GetAt(arena, type, index) ((type *)_GetAt_(arena, sizeof(type), index))
#define GetLast(arena, type) ((type *)_GetLastElement_(arena, sizeof(type)))
#define PushStruct(arena, type) ((type *)_Push_(arena, sizeof(type)))  

#define PushSize(arena, size, type) ((type *)_Push_(arena, size))  
#define PopStruct(arena, type) ((type *)_Pop_(arena, sizeof(type)))  

#define PushArray(arena, count, type) ((type *)_Push_(arena, (count) * sizeof(type)))
#define PopArray(arena, count, type) ((type *)_Pop_(arena, (count) * sizeof(type)))
#define PrintMessageBox(msg) MessageBoxA(0, msg, 0, 0)

#define ArrayCount(a) sizeof(a) / sizeof(*a)

// Alignments.

#define ALIGNUP(address, alignment) \
    (((size_t)address) + ((alignment)-1) & (~((alignment)-1)))

#define MISALIGNAMOUNT(address, alignment) \
    ((((size_t)alignment)-1) & address)

#define ALIGNDOWN(address, alignment) \
    (((address - ((((size_t)alignment)-1) & address))))

#ifdef _DEBUG
#define CheckMemory(cond) do { if (!(cond)) { MessageBoxA(0, "Out of memory in: " ##__FILE__, 0, 0); DebugBreak(); } } while(0)
#else
#define CheckMemory(cond)
#endif
