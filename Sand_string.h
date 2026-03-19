#pragma once

#include <stdint.h>

typedef struct
{
   char*   data;
   int32_t size;
   int32_t capacity;
} Sand_string_t;

//------------------------------------------------------------------------------
void sand_grow_string( Sand_string_t* string, int32_t len );

//------------------------------------------------------------------------------
void sand_string_create( Sand_string_t* string );

//------------------------------------------------------------------------------
void sand_string_destroy( Sand_string_t* string );

//------------------------------------------------------------------------------
void sand_string_append( Sand_string_t* string, const char* str );
