#pragma once


// sand_string_view
typedef struct
{
   char*  data;
   size_t size;
} sand_string_view_t;

void sand_string_view_trim( sand_string_view_t* sv, char ch );
void sand_string_view_left_trim( sand_string_view_t* sv, char ch );
void sand_string_view_right_trim( sand_string_view_t* sv, char ch );

