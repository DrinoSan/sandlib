#ifndef ATOM_INCLUDED
#define ATOM_INCLUDED


// INTERFACE for ATOM improvement of Strings

// ----------------------------------------------------------------------------
int sand_atom_length( const char* str );

// ----------------------------------------------------------------------------
const char* sand_atom_new( const char* str, int len );

// ----------------------------------------------------------------------------
const char* sand_atom_string( const char* str );

// ----------------------------------------------------------------------------
const char* sand_atom_int( long n );

#endif
