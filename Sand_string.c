#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Sand_string.h"

//------------------------------------------------------------------------------
void sand_grow_string( Sand_string_t* string, int32_t len )
{
   if( (int32_t)(string->capacity * 2 ) <=  (int32_t)( string->size + len ) )
   {
      // I need to allocate more than just * 2
      string->capacity = string->capacity + len * 2;
      string->data = realloc( string->data, string->capacity );
      assert( string->data != NULL );
      return;
   }

   string->capacity *= 2;
   string->data = realloc( string->data, string->capacity );
   assert( string->data != NULL );
}

//------------------------------------------------------------------------------
void sand_string_create( Sand_string_t* string )
{
   memset( string, 0, sizeof( *string ) );
   string->data = NULL;
   string->size = 0;
   string->capacity = 0;
}

//------------------------------------------------------------------------------
void sand_string_destroy( Sand_string_t* string )
{
   free( string->data );
   string->data = NULL;
   string->size = 0;
   string->capacity = 0;
}

//------------------------------------------------------------------------------
void sand_string_append( Sand_string_t* string, const char* str )
{
   const int32_t len = strlen( str );
   if( (int32_t)( string->size + len ) >= string->capacity )
   {
      sand_grow_string( string, len );
   }

   if( string->size == 0 )
   {
      // Empty string we strcpy
      strcpy( string->data, str );
      string->size += len;
      return;
   }

   strcat( string->data, str );
   string->size += len;
}
