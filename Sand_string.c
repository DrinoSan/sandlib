#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Sand_string.h"
#include "mem.h"

//------------------------------------------------------------------------------
void sand_grow_string( Sand_string_t* string, int32_t len )
{
   if ( string->capacity == 0 )
   {
      // Need to call alloc first because RESIZE does not like a NULL ptr
      string->capacity = 10;
      string->data     = ALLOC( string->capacity );
   }

   if ( ( int32_t ) ( string->capacity * 2 ) <=
        ( int32_t ) ( string->size + len ) )
   {
      // I need to allocate more than just * 2
      string->capacity = string->capacity + len * 2;
   }
   else
   {
      string->capacity *= 2;
   }

   RESIZE( string->data, string->capacity );
   assert( string->data != NULL );
}

//------------------------------------------------------------------------------
void sand_string_create( Sand_string_t* string )
{
   memset( string, 0, sizeof( *string ) );
   string->data     = NULL;
   string->size     = 0;
   string->capacity = 0;
}

//------------------------------------------------------------------------------
void sand_string_destroy( Sand_string_t* string )
{
   free( string->data );
   string->data     = NULL;
   string->size     = 0;
   string->capacity = 0;
}

//------------------------------------------------------------------------------
void sand_string_append( Sand_string_t* string, const char* str )
{
   sand_string_append_n( string, str, strlen( str ) );
}

//------------------------------------------------------------------------------
void sand_string_append_n( Sand_string_t* string, const char* str, size_t len )
{
   // I dont want strings of size 0 or smaller
   //assert( len > 0 );
   if( len <= 0 )
   {
      return;
   }

   if ( ( int32_t ) ( string->size + len ) >= string->capacity )
   {
      sand_grow_string( string, len );
   }

   if ( string->size == 0 )
   {
      // Empty string we strcpy
      strcpy( string->data, str );
      string->size += len;
      return;
   }

   memcpy( string->data + string->size, str, len );
   string->size += len;
}

//------------------------------------------------------------------------------
bool sand_string_has_prefix( Sand_string_t* string, const char* prefix )
{
   size_t prefix_len = strlen( prefix );

   return sand_string_has_prefix_n( string, prefix, prefix_len );
}

//------------------------------------------------------------------------------
bool sand_string_has_prefix_n( Sand_string_t* string, const char* prefix,
                               size_t len )
{
   if ( ( size_t ) string->size < len )
   {
      return false;
   }

   for ( size_t i = 0; i < len; i++ )
   {
      if ( string->data[ i ] != prefix[ i ] )
      {
         return false;
      }
   }

   return true;
}

//------------------------------------------------------------------------------
bool sand_string_has_suffix( Sand_string_t* string, const char* prefix )
{
   size_t suffix_len = strlen( prefix );

   return sand_string_has_suffix_n( string, prefix, suffix_len );
}

//------------------------------------------------------------------------------
bool sand_string_has_suffix_n( Sand_string_t* string, const char* prefix,
                               size_t len )
{
   if ( ( size_t ) string->size < len )
   {
      return false;
   }

   size_t i          = ( size_t ) string->size - len;
   size_t suffix_idx = 0;
   for ( ; i < ( size_t ) string->size; i++ )
   {
      if ( string->data[ i ] != prefix[ suffix_idx ] )
      {
         return false;
      }

      suffix_idx++;
   }

   return true;
}

//------------------------------------------------------------------------------
bool sand_string_has_substr( Sand_string_t* string, const char* substr )
{
   return sand_string_has_substr_n( string, substr, strlen( substr ) );
}

//------------------------------------------------------------------------------
bool sand_string_has_substr_n( Sand_string_t* string, const char* substr,
                               size_t substr_len )
{
   if ( ( size_t ) string->size < substr_len )
   {
      return false;
   }

   if ( substr_len == 0 )
   {
      return true;
   }

   size_t limit = ( size_t ) string->size - substr_len;
   for ( size_t i = 0; i <= limit; i++ )
   {
      if ( memcmp( string->data + i, substr, substr_len ) == 0 )
      {
         return true;
      }
   }

   return false;
}
//------------------------------------------------------------------------------
void sand_string_left_trim( Sand_string_t* string, char ch )
{
   const char* begin = string->data;
   // H e l l o \0 = 6

   if ( ch == 0 )
   {
      while ( isspace( ( unsigned char ) *begin ) )
      {
         begin++;
      }
   }
   else
   {
      while ( *begin == ch )
      {
         begin++;
      }
   }

   // All whitespace
   if ( *begin == 0 )
   {
      string->data[ 0 ] = 0;
      string->size      = 0;
      return;
   }

   size_t len = strlen( begin );
   memmove( string->data, begin, len + 1 );
   string->size = len;
}

//------------------------------------------------------------------------------
void sand_string_right_trim( Sand_string_t* string, char ch )
{
   const char* end = string->data + strlen( string->data ) - 1;

   if ( ch == 0 )
   {
      while ( end >= string->data && isspace( ( unsigned char ) *end ) )
      {
         end--;
      }
   }
   else
   {
      while ( end >= string->data && *end == ch )
      {
         end--;
      }
   }

   if ( end < string->data )
   {
      string->data[ 0 ] = 0;
      string->size      = 0;
      return;
   }

   size_t new_len              = end - string->data;
   string->data[ new_len + 1 ] = 0;
   string->size                = new_len + 1;
}

//------------------------------------------------------------------------------
void sand_string_trim( Sand_string_t* string, char ch )
{
   sand_string_left_trim( string, ch );

   if ( string->size == 0 )
   {
      return;
   }

   sand_string_right_trim( string, ch );
}

//------------------------------------------------------------------------------
void sand_string_left_trim_cstr( char* string, char ch )
{
   if ( *string == '\0' )
   {
      return;
   }

   const char* begin = string;
   // H e l l o \0 = 6

   if ( ch == 0 )
   {
      while ( isspace( ( unsigned char ) *begin ) )
      {
         begin++;
      }
   }
   else
   {
      while ( *begin == ch )
      {
         begin++;
      }
   }

   // All whitespace
   if ( *begin == 0 )
   {
      string[ 0 ] = '\0';
      return;
   }

   size_t len = strlen( begin );
   memmove( string, begin, len + 1 );
}

//------------------------------------------------------------------------------
void sand_string_right_trim_cstr( char* string, char ch )
{
   if ( *string == '\0' )
   {
      return;
   }

   const char* end = string + strlen( string ) - 1;

   if ( ch == 0 )
   {
      while ( end >= string && isspace( ( unsigned char ) *end ) )
      {
         end--;
      }
   }
   else
   {
      while ( end >= string && *end == ch )
      {
         end--;
      }
   }

   if ( end < string )
   {
      string[ 0 ] = '\0';
      return;
   }

   size_t new_len        = end - string;
   string[ new_len + 1 ] = 0;
}

//------------------------------------------------------------------------------
void sand_string_trim_cstr( char* string, char ch )
{
   sand_string_left_trim_cstr( string, ch );
   if ( strlen( string ) == 0 )
   {
      return;
   }
   sand_string_right_trim_cstr( string, ch );
}
