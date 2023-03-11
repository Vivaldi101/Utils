// functions.h
#ifndef DEF
#define DEF(...)
#endif

DEF(int, Sum, int a, int b);
DEF(void, Print, const char *fmt, ...);
DEF(void*, Alloc, size_t size);

#undef DEF

//
// file.c
//
#define DEF(Type, Name, ...) Type (*Name)(__VA_ARGS__)
struct VTable {
#include "functions.h"
};

#define DEF(Type, Name, ...) table->name = (Type(*)(__VA_ARGS__))GetProcAddress(library, #Name)
void LoadVTable(VTable *table, HMODULE library) {
#include "functions.h"
}
