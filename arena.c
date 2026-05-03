#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "arena.h"
#include "except.h"

#define T sand_arena_t

#define THRESHOLD 10

const sand_except_t sand_arena_new_failed = { "Arena Creation Failed" };
const sand_except_t sand_arena_failed     = { "Arena Allocation Failed" };

// checking types
union align
{
   int   i;
   long  l;
   long* lp;
   void* p;
   void ( *fp )( void );
   float       f;
   double      d;
   long double ld;
};

union header
{
   struct T*   b;
   union align a;
};

struct T
{
   T*    prev;    // Pointing to the next arena memory chunk
   char* avail;   // Pointing to the chunks first free memory
   char* limit;   // Pointing past the chunks end
};

static T*  freechunks;
static int nfree;

T* sand_arena_new( void )
{
   T* arena = malloc( sizeof( *arena ) );

   if ( arena == NULL )
   {
      RAISE( sand_arena_new_failed );
   }

   arena->prev  = NULL;
   arena->limit = arena->avail = NULL;

   return arena;
}

void sand_arena_destroy( T** ap )
{
   assert( ap && *ap );

   sand_arena_free( *ap );
   free( *ap );
   *ap = NULL;
}

void* sand_arena_alloc( T* arena, long nbytes, const char* file, int line )
{
   assert( arena );
   assert( nbytes > 0 );

   // round nbytes
   nbytes =
       ( ( nbytes + sizeof( union align ) - 1 ) / ( sizeof( union align ) ) ) *
       ( sizeof( union align ) );

   while ( nbytes > arena->limit - arena->avail )
   {
      // get a new chunk
      T*    ptr;
      char* limit;
      if ( ( ptr = freechunks ) != NULL )
      {
         freechunks = freechunks->prev;
         nfree--;
         limit = ptr->limit;
      }
      else
      {
         long m = sizeof( union header ) + nbytes + 10 * 1024;
         ptr    = malloc( m );
         if ( ptr == NULL )
         {
            // raise
            if ( file == NULL )
            {
               RAISE( sand_arena_failed );
            }
            else
            {
               sand_except_raise( &sand_arena_failed, file, line );
            }
         }

         limit = ( char* ) ptr + m;
      }

      // Big brain move
      // The client only ever holds the *arena variable and we never need to
      // return a new arena. If we created a new one we just replace it and save
      // the old state in prev
      *ptr         = *arena;
      arena->avail = ( char* ) ( ( union header* ) ptr + 1 );
      arena->limit = limit;
      arena->prev  = ptr;
   }

   arena->avail += nbytes;

   return arena->avail - nbytes;
}

void* sand_arena_calloc( T* arena, long count, long nbytes, const char* file, int line )
{
   void* ptr;

   assert( count > 0 );
   ptr = sand_arena_alloc( arena, count*nbytes, file, line );
   memset( ptr, '\0', count*nbytes );

   return ptr;
}

void sand_arena_free( T* arena )
{
   assert( arena );

   while( arena->prev )
   {
      struct T tmp = *arena->prev;
      // free the chunk
      if( nfree < THRESHOLD )
      {
         arena->prev->prev = freechunks;
         freechunks = arena->prev;
         nfree++;
         freechunks->limit = arena->limit;
      }
      else
      {
         free( arena->prev );
      }
      *arena = tmp;
   }

   assert( arena->limit == NULL );
   assert( arena->avail == NULL );
}
