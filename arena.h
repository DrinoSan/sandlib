#ifndef ARENA_INCLUDED
#define ARENA_INCLUDED
#include "except.h"

// Opaque pointer, the actual definition is in the implementation file
#define T sand_arena_t
typedef struct T T;

extern const sand_except_t sand_arena_new_failed;
extern const sand_except_t sand_arena_failed;

T*    sand_arena_new( void );

// T** needed because i am setting the pointer to NULL to avoid dangeling pointers
void sand_arena_destroy( T** ap );

void* sand_arena_alloc( T* arena, long nbytes, const char* file, int line );

void* sand_arena_calloc( T* arena, long count, long nbytes, const char* file,
                         int line );

void sand_arena_free( T* arena );

#undef T
#endif
