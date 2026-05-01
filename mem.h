#ifndef MEM_INCLUDE
#define MEM_INCLUDE
// #include "except.h"

extern const sand_except_t Mem_Failed;

void* sand_mem_alloc( long nbytes, const char* file, int line );

void* sand_mem_calloc( long count, long nbytes, const char* file, int line );

void sand_mem_free( void* ptr, const char* file, int line );

void* sand_mem_resize( void* ptr, long nbytes, const char* file, int line );

#define ALLOC( nbytes ) sand_mem_alloc( nbytes, __FILE__, __LINE__ )

#define CALLOC( count, nbytes ) sand_mem_calloc( nbytes, __FILE__, __LINE__ )

#define NEW( p ) ( ( p ) = ALLOC( ( long ) sizeof *( p ) ) )

#define NEWO( p ) ( ( p ) = CALLOC( 1, ( long ) sizeof *( p ) ) )

#define FREE(ptr) ((void)(sand_mem_free((ptr), __FILE__, __LINE__), (ptr) = 0 ))

#define RESIZE( ptr, nbytes ) ((ptr) = sand_meme_resize( (ptr), (nbytes), __FILE__, __LINE__ ) )

#endif
