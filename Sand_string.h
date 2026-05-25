#pragma once

#include <stdint.h>
#include <stdbool.h>

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
bool sand_string_has_prefix( Sand_string_t* string, const char* prefix );
bool sand_string_has_prefix_n( Sand_string_t* string, const char* prefix, size_t len );

//------------------------------------------------------------------------------
bool sand_string_has_suffix( Sand_string_t* string, const char* prefix );
bool sand_string_has_suffix_n( Sand_string_t* string, const char* prefix, size_t len );

//------------------------------------------------------------------------------
bool sand_string_has_substr( Sand_string_t* string, const char* substr );
bool sand_string_has_substr_n( Sand_string_t* string, const char* substr, size_t substr_len );

//------------------------------------ TRIM ------------------------------------
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


