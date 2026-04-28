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

//------------------------------------------------------------------------------
void sand_string_append_n( Sand_string_t* string, const char* str, size_t len );

//------------------------------------------------------------------------------
void sand_string_left_trim( Sand_string_t* string, char ch );

//------------------------------------------------------------------------------
void sand_string_right_trim( Sand_string_t* string, char ch );

//------------------------------------------------------------------------------
void sand_string_trim( Sand_string_t* string, char ch );

//------------------------------------------------------------------------------
void sand_string_left_trim_cstr( char* string, char ch );

//------------------------------------------------------------------------------
void sand_string_right_trim_cstr( char* string, char ch );

//------------------------------------------------------------------------------
void sand_string_trim_cstr( char* string, char ch );


