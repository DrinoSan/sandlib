#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
// #include "exception.h"
#include "mem.h"

// const sand_except_t sand_mem_failed = { "Allocation Failed" };

void* sand_mem_alloc( long nbytes, const char* file, int line )
{
   void* ptr;

   assert( nbytes > 0 );

   ptr = malloc( nbytes );
   if ( ptr == NULL )
   {
      // raise sand_mem_failed
      if ( file == NULL )
      {
         // RAISE(sand_mem_failed);
      }
      else
      {
         // sand_except_raise( &sand_mem_failed, file, line );
      }
   }

   return ptr;
}

void* sand_mem_calloc( long count, long nbytes, const char* file, int line )
{
   void* ptr;

   assert( count > 0 );
   assert( nbytes > 0 );

   ptr = calloc( count, nbytes );
   if ( ptr == NULL )
   {
      if ( file == NULL )
      {
         // RAISE(sand_mem_failed);
      }
      else
      {
         // sand_except_raise( &sand_mem_failed, file, line );
      }
   }

   return ptr;
}

void sand_meme_free( void* ptr, const char* file, int line )
{
   if ( ptr )
   {
      free( ptr );
   }
}

void* sand_mem_resize( void* ptr, long nbytes, const char* file, int line )
{
   assert( ptr );
   assert( nbytes > 0 );

   ptr = realloc( ptr, nbytes );
   if( ptr == NULL )
   {
      if ( file == NULL )
      {
         // RAISE(sand_mem_failed);
      }
      else
      {
         // sand_except_raise( &sand_mem_failed, file, line );
      }
   }

   return ptr;
}
