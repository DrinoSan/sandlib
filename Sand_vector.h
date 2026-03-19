#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Example of freeElem for char*
void string_free( void** elem );

// Implementation of list
typedef struct
{
   void*  data;                   // generic data
   size_t size;                   // number of elements inside list
   size_t capacity;               // Number of potential elements
   size_t type_size;              // Size of single element
   void ( *freeElem )( void* );   // Needed if we hold char* elements which are
                                  // pointers to allocated space
} sand_list;

//-----------------------------------------------------------------------------
/// Function to push element e to the end of list l
/// @param: l sand_list is the destination
/// @param: e generic element defined by list type_size which will be pushe to
/// the end of the list
sand_list* sand_list_push_back( sand_list* l, void* e );

//-----------------------------------------------------------------------------
/// Function to remove last element from list
/// If list is empty 0 is returned
/// If out element is set it the popped element will be written to it
int sand_list_pop_back( sand_list* l, void* out_element );

//-----------------------------------------------------------------------------
/// Function to get a copy of element at index
/// Copy is written to out_element
/// Returns 0 if index is out of bounds otherwise 1
int sand_list_get( sand_list* l, int index, void* out_element );

//-----------------------------------------------------------------------------
/// Function to get a pointer to element at index
/// Returns pointer to element if sucessfull otherweise NULL
void* sand_list_at( sand_list* l, int index );

//-----------------------------------------------------------------------------
sand_list* sand_list_clear( sand_list* l );

//-----------------------------------------------------------------------------
sand_list* sand_list_shrink_to_fit( sand_list* l );

//-----------------------------------------------------------------------------
/// Function to create a empty list
/// If memory allocation fails there is no need to continue
/// @param: type_size needed for generic work
/// @return: Pointer to empty list
sand_list* sand_list_create( size_t type_size,
                              void ( *freeElem )( void* ) );

//-----------------------------------------------------------------------------
void sand_list_free( sand_list* l );
