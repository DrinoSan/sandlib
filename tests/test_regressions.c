// Regression tests for memory-safety bugs found in review (Commit 0b797dc).
// These tests are EXPECTED TO FAIL until the bugs are fixed.
// Best run with: make test-asan  (AddressSanitizer + UBSan)
#include "unity.h"

#include <limits.h>
#include <string.h>

#include "../Sand_string.h"
#include "../arena.h"
#include "../except.h"

void setUp( void ) { sand_except_stack = NULL; }
void tearDown( void ) { sand_except_stack = NULL; }

// Sand_string.c:86 uses strcpy on an empty string and copies up to the
// source's '\0' instead of exactly len bytes.
void test_append_n_on_empty_copies_only_len_bytes( void )
{
   Sand_string_t s;
   sand_string_create( &s );
   sand_string_append_n( &s, "abcXYZ", 3 );

   TEST_ASSERT_EQUAL_INT32( 3, s.size );
   TEST_ASSERT_EQUAL_STRING( "abc", s.data );
   sand_string_destroy( &s );
}

// Same bug, but the source is much longer than the buffer: strcpy writes
// past the end of the heap buffer (ASan: heap-buffer-overflow).
void test_append_n_on_empty_does_not_overflow_with_long_source( void )
{
   char src[ 256 ];
   memset( src, 'A', sizeof( src ) - 1 );
   src[ sizeof( src ) - 1 ] = '\0';

   Sand_string_t s;
   sand_string_create( &s );
   sand_string_append_n( &s, src, 1 );

   TEST_ASSERT_EQUAL_INT32( 1, s.size );
   TEST_ASSERT_EQUAL_CHAR( 'A', s.data[ 0 ] );
   sand_string_destroy( &s );
}

// After append_n into a non-empty string the result must stay
// NUL-terminated, because trim/has_* rely on strlen.
void test_append_n_keeps_string_nul_terminated( void )
{
   Sand_string_t s;
   sand_string_create( &s );
   sand_string_append( &s, "ab" );
   sand_string_append_n( &s, "cdef", 2 );

   TEST_ASSERT_EQUAL_INT32( 4, s.size );
   TEST_ASSERT_EQUAL_CHAR( '\0', s.data[ s.size ] );
   sand_string_destroy( &s );
}

// arena.c:130: count * nbytes overflows without a check. With these values
// the product wraps to 16, so the caller gets 16 bytes while believing it
// owns ~2^64. Expected: sand_arena_failed is raised.
void test_arena_calloc_rejects_count_times_size_overflow( void )
{
   sand_arena_t* a = sand_arena_new();
   long count      = ( 1L << 60 ) + 1;
   long nbytes     = 16;
   volatile int raised = 0;

   TRY
      sand_arena_calloc( a, count, nbytes, __FILE__, __LINE__ );
   EXCEPT( sand_arena_failed )
      raised = 1;
   END_TRY;

   TEST_ASSERT_TRUE_MESSAGE( raised, "overflow in count * nbytes not detected" );
   sand_arena_destroy( &a );
}

int main( void )
{
   UNITY_BEGIN();
   RUN_TEST( test_append_n_on_empty_copies_only_len_bytes );
   RUN_TEST( test_append_n_on_empty_does_not_overflow_with_long_source );
   RUN_TEST( test_append_n_keeps_string_nul_terminated );
   RUN_TEST( test_arena_calloc_rejects_count_times_size_overflow );
   return UNITY_END();
}
