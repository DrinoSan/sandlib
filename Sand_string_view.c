#include <ctype.h>
#include <stddef.h>
#include <stdio.h>

#include "Sand_string_view.h"

//------------------------------------------------------------------------------
void sand_string_view_reset( sand_string_view_t* sv )
{
   sv->data = NULL;
   sv->size = 0;
}

//------------------------------------------------------------------------------
void sand_string_view_left_trim( sand_string_view_t* sv, char ch )
{
   if ( sv->size == 0 )
   {
      return;
   }

   char* begin = sv->data;
   // H e l l o \0 = 6

   if ( ch == 0 )
   {
      while ( isspace( ( unsigned char ) *begin ) )
      {
         begin++;
         sv->size--;
      }
   }
   else
   {
      while ( *begin == ch )
      {
         begin++;
         sv->size--;
      }
   }

   // All whitespace
   if ( *begin == 0 )
   {
      sv->data = NULL;
      sv->size = 0;
      return;
   }

   sv->data = begin;
}

//------------------------------------------------------------------------------
void sand_string_view_right_trim( sand_string_view_t* sv, char ch )
{
   if ( sv->size == 0 )
   {
      return;
   }

   // -1 because if word has 4 letter like "SAND" then sv->data[sv->size] would
   // now be the last char but the char after that which is UB and also start
   // index is 0 therefore -1 is needed
   char* end = sv->data + sv->size - 1;

   if ( ch == 0 )
   {
      while ( end >= sv->data && isspace( ( unsigned char ) *end ) )
      {
         end--;
         sv->size--;
      }
   }
   else
   {
      while ( end >= sv->data && *end == ch )
      {
         end--;
         sv->size--;
      }
   }

   if ( end < sv->data )
   {
      sv->data = NULL;
      sv->size = 0;

      return;
   }
}

//------------------------------------------------------------------------------
void sand_string_view_trim( sand_string_view_t* sv, char ch )
{
   sand_string_view_left_trim( sv, ch );
   if ( sv->size == 0 )
   {
      return;
   }
   sand_string_view_right_trim( sv, ch );
}

//------------------------------------------------------------------------------
bool sand_string_view_has_substr( const sand_string_view_t* sv,
                                  const char*               substr )
{
   size_t substr_len = strlen( substr );
   if ( ( size_t ) sv->size < substr_len )
   {
      return false;
   }

   if ( substr_len == 0 )
   {
      return true;
   }

   size_t limit = ( size_t ) sv->size - substr_len;
   for ( size_t i = 0; i <= limit; i++ )
   {
      if ( memcmp( sv->data + i, substr, substr_len ) == 0 )
      {
         return true;
      }
   }

   return false;
}

//------------------------------------------------------------------------------
static char sand_string_view_to_lower( char ch )
{
   if ( ch >= 'A' && ch <= 'Z' )
   {
      return ch | 0x20;
   }

   return ch;
}

//------------------------------------------------------------------------------
bool sand_string_view_has_substr_no_case( const sand_string_view_t* sv,
                                          const char*               substr )
{
   size_t substr_len = strlen( substr );
   if ( ( size_t ) sv->size < substr_len )
   {
      return false;
   }

   if ( substr_len == 0 )
   {
      return true;
   }

   size_t limit = ( size_t ) sv->size - substr_len;
   for ( size_t i = 0; i <= limit; i++ )
   {
      bool found = true;
      for ( size_t j = 0; j < substr_len; ++j )
      {
         if ( sand_string_view_to_lower( sv->data[ i + j ] ) !=
              sand_string_view_to_lower( substr[ j ] ) )
         {
            found = false;
            break;
         }
      }

      if ( found == true )
      {
         return true;
      }
   }

   return false;
}
