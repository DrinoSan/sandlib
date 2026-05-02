#include "except.h"
#include "mem.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

// checking macros
#define hash( p, t )                                                           \
   ( ( ( unsigned long ) ( p ) >> 4 ) &                                        \
     ( sizeof( t ) / sizeof( ( t )[ 0 ] ) - 1 ) )

#define NDESCRIPTORS 512

#define NALLOC                                                                 \
   ( ( 4096 + sizeof( union align ) - 1 ) / ( sizeof( union align ) ) ) *      \
       ( sizeof( union align ) )

// data
const sand_except_t sand_mem_failed = { "Allocation Failed" };

// checking data
static struct descriptor
{
   struct descriptor* free;
   struct descriptor* link;
   const void*        ptr;
   long               size;
   const char*        file;
   int                line;
}* htab[ 2048 ];

static struct descriptor freelist = { &freelist };
// checking functions
static struct descriptor* find( const void* ptr )
{
   struct descriptor* bp = htab[ hash( ptr, htab ) ];

   while ( bp && bp->ptr != ptr )
   {
      bp = bp->link;
   }

   return bp;
}

void sand_mem_free( void* ptr, const char* file, int line )
{
   if ( ptr )
   {
      struct descriptor* bp = NULL;

      if ( ( ( uintptr_t ) ptr ) % ( sizeof( union align ) ) != 0 ||
           ( bp = find( ptr ) ) == NULL || bp->free )
      {
         sand_except_raise( &sand_mem_failed, file, line );
         // sand_except_raise( &sand_assert_faild, file, line );
      }

      bp->free      = freelist.free;
      freelist.free = bp;
   }
}

void* sand_mem_resize( void* ptr, long nbytes, const char* file, int line )
{
   struct descriptor* bp     = NULL;
   void*              newptr = NULL;

   assert( ptr );
   assert( nbytes > 0 );

   if ( ( ( uintptr_t ) ptr ) % ( sizeof( union align ) ) != 0 ||
        ( bp = find( ptr ) ) == NULL || bp->free )
   {
      sand_except_raise( &sand_mem_failed, file, line );
      // sand_except_raise( &sand_assert_faild, file, line );
   }

   newptr = sand_mem_alloc( nbytes, file, line );
   memcpy( newptr, ptr, nbytes < bp->size ? nbytes : bp->size );

   sand_mem_free( ptr, file, line );

   return newptr;
}

void* sand_mem_calloc( long count, long nbytes, const char* file, int line )
{
   void* ptr = NULL;

   assert( count > 0 );
   assert( nbytes > 0 );

   ptr = sand_mem_alloc( count * nbytes, file, line );
   memset( ptr, '\0', count * nbytes );

   return ptr;
}

static struct descriptor* dalloc( void* ptr, long size, const char* file,
                                  int line )
{
   static struct descriptor* avail = NULL;
   static int                nleft;

   if ( nleft <= 0 )
   {
      // allocate descriptors
      avail = malloc( NDESCRIPTORS * sizeof( *avail ) );
      if ( avail == NULL )
      {
         return NULL;
      }

      nleft = NDESCRIPTORS;
   }

   avail->ptr  = ptr;
   avail->size = size;
   avail->file = file;
   avail->line = line;
   avail->free = avail->link = NULL;
   nleft--;

   return avail++;
}

void* sand_mem_alloc( long nbytes, const char* file, int line )
{
   struct descriptor* bp  = NULL;
   void*              ptr = NULL;

   assert( nbytes > 0 );
   // round
   nbytes =
       ( ( nbytes + sizeof( union align ) - 1 ) / ( sizeof( union align ) ) ) *
       ( sizeof( union align ) );

   for ( bp = freelist.free; bp; bp = bp->free )
   {
      if ( bp->size > nbytes )
      {
         bp->size -= nbytes;
         ptr = ( char* ) bp->ptr + bp->size;
         if ( ( bp = dalloc( ptr, nbytes, file, line ) ) != NULL )
         {
            unsigned h = hash( ptr, htab );
            bp->link   = htab[ h ];
            htab[ h ]  = bp;

            return ptr;
         }
         else
         {
            // raise sand_mem_faild
            if ( file == NULL )
            {
               RAISE( sand_mem_failed );
            }
            else
            {
               sand_except_raise( &sand_mem_failed, file, line );
            }
         }
      }

      if ( bp == &freelist )
      {
         struct descriptor* newptr = NULL;
         // nalloc
         if ( ( ptr = malloc( nbytes + NALLOC ) ) == NULL ||
              ( newptr = dalloc( ptr, nbytes + NALLOC, __FILE__, __LINE__ ) ) ==
                  NULL )
         {
            // raise sand_mem_failed
            if ( file == NULL )
            {
               RAISE( sand_mem_failed );
            }
            else
            {
               sand_except_raise( &sand_mem_failed, file, line );
            }
         }

         newptr->free  = freelist.free;
         freelist.free = newptr;
      }
   }

   assert( 0 );
   return NULL;
}
