#include <stddef.h>
#include <ctype.h>

#include "Sand_string_view.h"

//------------------------------------------------------------------------------
void sand_string_view_left_trim( sand_string_view_t* sv, char ch )
{
   if ( *sv->data == '\0' )
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
   if ( *(sv->data + sv->size) == '\0' )
   {
      return;
   }

   char* end = sv->data + sv->size;

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
