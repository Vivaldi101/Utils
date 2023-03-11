// List macros.

#define OffsetOf(type, field) (size_t)&((type*)0)->field
#define ContainerOf(ptr, type, field) (type*)(((char*)(ptr)) - OffsetOf(type, field))
#define ListEntry(ptr, type, field) ContainerOf(ptr, type, field)

#define ListCreate(listHead) (listHead)->previous = (listHead)->next = (listHead); 

#define ListForEach(cursor, listHead, type, field) \
		for (type* (cursor) = ListEntry((listHead)->next, type, field); \
			&(cursor)->field != (listHead); \
			(cursor) = ListEntry((cursor)->field.next, type, field))

