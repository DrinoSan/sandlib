#include <string.h>

// SandLib Headers
#include "sand_file.h"

void sand_file_create( sand_file_t* file_handler )
{
   memset( file_handler, 0, sizeof( *file_handler ) );
   sand_string_create( &file_handler->content );
}

// Function to open a file and retrieve a file descriptor
void sand_file_open_and_read( sand_file_t* file_handler, const char* file_path )
{
   file_handler->fp = fopen( file_path, "r" );
   if ( file_handler->fp == NULL )
   {
      printf( "Cant read file, check path!\n" );
      return;
   }

   fseek( file_handler->fp, 0, SEEK_END );
   size_t file_size = ftell( file_handler->fp );
   fseek( file_handler->fp, 0, SEEK_SET );

   sand_grow_string( &file_handler->content, file_size );
   fread( file_handler->content.data, file_size, 1, file_handler->fp );
   file_handler->content.size = file_size;

   fclose( file_handler->fp );
}

