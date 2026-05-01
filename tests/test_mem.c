#include "unity.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "../except.h"
#include "../mem.h"

// Defined in mem.c but not exported by mem.h (commented out)
extern const sand_except_t sand_mem_failed;

void setUp( void )
{
    sand_except_stack = NULL;
}

void tearDown( void )
{
    sand_except_stack = NULL;
}

// =============================================================================
// sand_mem_alloc (production)
// =============================================================================

void test_alloc_returns_non_null( void )
{
    void* p = sand_mem_alloc( 100, __FILE__, __LINE__ );

    TEST_ASSERT_NOT_NULL( p );
    free( p );
}

void test_alloc_memory_is_writable( void )
{
    char* p = sand_mem_alloc( 64, __FILE__, __LINE__ );

    memset( p, 'A', 64 );
    TEST_ASSERT_EQUAL_CHAR( 'A', p[ 0 ] );
    TEST_ASSERT_EQUAL_CHAR( 'A', p[ 63 ] );

    free( p );
}

void test_alloc_various_sizes( void )
{
    // Small allocation
    void* p1 = sand_mem_alloc( 1, __FILE__, __LINE__ );
    TEST_ASSERT_NOT_NULL( p1 );

    // Medium allocation
    void* p2 = sand_mem_alloc( 4096, __FILE__, __LINE__ );
    TEST_ASSERT_NOT_NULL( p2 );

    // Larger allocation
    void* p3 = sand_mem_alloc( 65536, __FILE__, __LINE__ );
    TEST_ASSERT_NOT_NULL( p3 );

    free( p1 );
    free( p2 );
    free( p3 );
}

// =============================================================================
// sand_mem_calloc (production)
// =============================================================================

void test_calloc_returns_zeroed_memory( void )
{
    unsigned char* p = sand_mem_calloc( 1, 128, __FILE__, __LINE__ );

    TEST_ASSERT_NOT_NULL( p );

    for ( int i = 0; i < 128; i++ )
    {
        TEST_ASSERT_EQUAL_UINT8( 0, p[ i ] );
    }

    free( p );
}

void test_calloc_count_times_size( void )
{
    // 10 elements of 4 bytes each = 40 bytes, all zeroed
    int* p = sand_mem_calloc( 10, sizeof( int ), __FILE__, __LINE__ );

    TEST_ASSERT_NOT_NULL( p );

    for ( int i = 0; i < 10; i++ )
    {
        TEST_ASSERT_EQUAL_INT( 0, p[ i ] );
    }

    free( p );
}

// =============================================================================
// sand_mem_resize (production)
// =============================================================================

void test_resize_preserves_data( void )
{
    char* p = sand_mem_alloc( 32, __FILE__, __LINE__ );

    memcpy( p, "hello world", 12 );

    p = sand_mem_resize( p, 64, __FILE__, __LINE__ );

    TEST_ASSERT_NOT_NULL( p );
    TEST_ASSERT_EQUAL_STRING( "hello world", p );

    free( p );
}

void test_resize_grows( void )
{
    int* p = sand_mem_alloc( 4 * sizeof( int ), __FILE__, __LINE__ );

    p[ 0 ] = 10;
    p[ 1 ] = 20;
    p[ 2 ] = 30;
    p[ 3 ] = 40;

    p = sand_mem_resize( p, 8 * sizeof( int ), __FILE__, __LINE__ );

    TEST_ASSERT_NOT_NULL( p );
    TEST_ASSERT_EQUAL_INT( 10, p[ 0 ] );
    TEST_ASSERT_EQUAL_INT( 20, p[ 1 ] );
    TEST_ASSERT_EQUAL_INT( 30, p[ 2 ] );
    TEST_ASSERT_EQUAL_INT( 40, p[ 3 ] );

    free( p );
}

// =============================================================================
// Exception on failure
// =============================================================================

void test_alloc_failure_raises_sand_mem_failed( void )
{
    volatile int caught = 0;

    TRY
        // Request an absurd amount that malloc cannot satisfy
        void* p = sand_mem_alloc( LONG_MAX, __FILE__, __LINE__ );
        (void) p;
    EXCEPT( sand_mem_failed )
        caught = 1;
    END_TRY;

    TEST_ASSERT_EQUAL_INT( 1, caught );
}

void test_calloc_failure_raises_sand_mem_failed( void )
{
    volatile int caught = 0;

    TRY
        void* p = sand_mem_calloc( LONG_MAX, LONG_MAX, __FILE__, __LINE__ );
        (void) p;
    EXCEPT( sand_mem_failed )
        caught = 1;
    END_TRY;

    TEST_ASSERT_EQUAL_INT( 1, caught );
}

// =============================================================================
// Main
// =============================================================================

int main( void )
{
    UNITY_BEGIN();

    // alloc
    RUN_TEST( test_alloc_returns_non_null );
    RUN_TEST( test_alloc_memory_is_writable );
    RUN_TEST( test_alloc_various_sizes );

    // calloc
    RUN_TEST( test_calloc_returns_zeroed_memory );
    RUN_TEST( test_calloc_count_times_size );

    // resize
    RUN_TEST( test_resize_preserves_data );
    RUN_TEST( test_resize_grows );

    // exception on failure
    RUN_TEST( test_alloc_failure_raises_sand_mem_failed );
    RUN_TEST( test_calloc_failure_raises_sand_mem_failed );

    return UNITY_END();
}
