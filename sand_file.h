#ifndef SAND_FILE_INCLUDED
#define SAND_FILE_INCLUDED

#include <stdio.h>


// SandLib Headers
#include "Sand_string.h"

typedef struct
{
   FILE* fp;
   Sand_string_t content;
} sand_file_t;

// Utilty file for reading file from filesystem

void sand_file_create( sand_file_t* file_handler );

// Function to open a file and retrieve a file descriptor
void sand_file_open_and_read( sand_file_t* file_handler, const char* file_path );

#endif   // SAND_FILE_H
