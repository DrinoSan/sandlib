#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "except.h"
#define T sand_except_t

sand_except_frame* sand_except_stack = NULL;

void sand_except_raise( const T* e, const char* file, int line )
{
   sand_except_frame* p = sand_except_stack;

   assert( e );
   if ( p == NULL )
   {
      fprintf( stderr, "Uncaught exception" );
      if ( e->reason )
      {
         fprintf( stderr, " %s", e->reason );
      }
      else
      {
         fprintf( stderr, " at %p", (void*)e );
      }

      if ( file && line > 0 )
      {
         fprintf( stderr, " raised at %s:%d\n", file, line );
      }

      fprintf( stderr, "aborting...\n" );
      fflush( stderr );
      abort();
   }

   p->exception      = e;
   p->file           = file;
   p->line           = line;
   sand_except_stack = sand_except_stack->prev;
   longjmp( p->env, sand_except_raised );
}
