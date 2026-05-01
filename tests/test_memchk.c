#include "unity.h"

#include <stdint.h>
#include <string.h>

#include "../except.h"
#include "../mem.h"

// Mirror the alignment union from memchk.c to verify alignment
union align
{
    int   i;
    long  l;
    long* lp;
    void* p;
    void ( *fp )( void );
    float       f;
    double      d;
    long double ld;
};

void setUp( void )
{
}

void tearDown( void )
{
}

// =============================================================================
// sand_mem_alloc (memchk -- checked allocator)
// =============================================================================

void test_chk_alloc_returns_non_null( void )
{
    void* p = sand_mem_alloc( 100, __FILE__, __LINE__ );

    TEST_ASSERT_NOT_NULL( p );
    sand_mem_free( p, __FILE__, __LINE__ );
}

void test_chk_alloc_returns_aligned_pointer( void )
{
    void* p = sand_mem_alloc( 1, __FILE__, __LINE__ );

    TEST_ASSERT_NOT_NULL( p );
    TEST_ASSERT_EQUAL_UINT64(
        0, ( uintptr_t ) p % sizeof( union align ) );

    sand_mem_free( p, __FILE__, __LINE__ );
}

void test_chk_alloc_memory_is_writable( void )
{
    char* p = sand_mem_alloc( 64, __FILE__, __LINE__ );

    memset( p, 'B', 64 );
    TEST_ASSERT_EQUAL_CHAR( 'B', p[ 0 ] );
    TEST_ASSERT_EQUAL_CHAR( 'B', p[ 63 ] );

    sand_mem_free( p, __FILE__, __LINE__ );
}

void test_chk_alloc_various_sizes( void )
{
    void* p1 = sand_mem_alloc( 1, __FILE__, __LINE__ );
    void* p2 = sand_mem_alloc( 100, __FILE__, __LINE__ );
    void* p3 = sand_mem_alloc( 4096, __FILE__, __LINE__ );

    TEST_ASSERT_NOT_NULL( p1 );
    TEST_ASSERT_NOT_NULL( p2 );
    TEST_ASSERT_NOT_NULL( p3 );

    // All should be aligned
    TEST_ASSERT_EQUAL_UINT64(
        0, ( uintptr_t ) p1 % sizeof( union align ) );
    TEST_ASSERT_EQUAL_UINT64(
        0, ( uintptr_t ) p2 % sizeof( union align ) );
    TEST_ASSERT_EQUAL_UINT64(
        0, ( uintptr_t ) p3 % sizeof( union align ) );

    sand_mem_free( p1, __FILE__, __LINE__ );
    sand_mem_free( p2, __FILE__, __LINE__ );
    sand_mem_free( p3, __FILE__, __LINE__ );
}

void test_chk_alloc_multiple_do_not_overlap( void )
{
    char* a = sand_mem_alloc( 64, __FILE__, __LINE__ );
    char* b = sand_mem_alloc( 64, __FILE__, __LINE__ );

    // Write different patterns
    memset( a, 'A', 64 );
    memset( b, 'B', 64 );

    // Verify neither corrupted the other
    for ( int i = 0; i < 64; i++ )
    {
        TEST_ASSERT_EQUAL_CHAR( 'A', a[ i ] );
        TEST_ASSERT_EQUAL_CHAR( 'B', b[ i ] );
    }

    sand_mem_free( a, __FILE__, __LINE__ );
    sand_mem_free( b, __FILE__, __LINE__ );
}

// =============================================================================
// sand_mem_calloc (memchk)
// =============================================================================

void test_chk_calloc_returns_zeroed( void )
{
    unsigned char* p = sand_mem_calloc( 1, 128, __FILE__, __LINE__ );

    TEST_ASSERT_NOT_NULL( p );

    for ( int i = 0; i < 128; i++ )
    {
        TEST_ASSERT_EQUAL_UINT8( 0, p[ i ] );
    }

    sand_mem_free( p, __FILE__, __LINE__ );
}

void test_chk_calloc_count_times_size( void )
{
    int* p = sand_mem_calloc( 10, sizeof( int ), __FILE__, __LINE__ );

    TEST_ASSERT_NOT_NULL( p );

    for ( int i = 0; i < 10; i++ )
    {
        TEST_ASSERT_EQUAL_INT( 0, p[ i ] );
    }

    sand_mem_free( p, __FILE__, __LINE__ );
}

// =============================================================================
// sand_mem_free (memchk)
// =============================================================================

void test_chk_free_and_realloc( void )
{
    // Allocate, free, then allocate again -- should not crash
    void* p1 = sand_mem_alloc( 128, __FILE__, __LINE__ );
    TEST_ASSERT_NOT_NULL( p1 );

    sand_mem_free( p1, __FILE__, __LINE__ );

    void* p2 = sand_mem_alloc( 64, __FILE__, __LINE__ );
    TEST_ASSERT_NOT_NULL( p2 );

    sand_mem_free( p2, __FILE__, __LINE__ );
}

void test_chk_free_null_is_noop( void )
{
    // Freeing NULL should not crash
    sand_mem_free( NULL, __FILE__, __LINE__ );
}

// =============================================================================
// sand_mem_resize (memchk)
// =============================================================================

void test_chk_resize_preserves_data( void )
{
    char* p = sand_mem_alloc( 32, __FILE__, __LINE__ );

    memcpy( p, "hello world", 12 );

    p = sand_mem_resize( p, 128, __FILE__, __LINE__ );

    TEST_ASSERT_NOT_NULL( p );
    TEST_ASSERT_EQUAL_INT( 0, memcmp( p, "hello world", 12 ) );

    sand_mem_free( p, __FILE__, __LINE__ );
}

void test_chk_resize_shrink( void )
{
    char* p = sand_mem_alloc( 256, __FILE__, __LINE__ );

    memcpy( p, "data", 5 );

    p = sand_mem_resize( p, 32, __FILE__, __LINE__ );

    TEST_ASSERT_NOT_NULL( p );
    TEST_ASSERT_EQUAL_INT( 0, memcmp( p, "data", 5 ) );

    sand_mem_free( p, __FILE__, __LINE__ );
}

void test_chk_resize_result_is_aligned( void )
{
    void* p = sand_mem_alloc( 16, __FILE__, __LINE__ );

    p = sand_mem_resize( p, 256, __FILE__, __LINE__ );

    TEST_ASSERT_EQUAL_UINT64(
        0, ( uintptr_t ) p % sizeof( union align ) );

    sand_mem_free( p, __FILE__, __LINE__ );
}

// =============================================================================
// Stress: many allocations
// =============================================================================

void test_chk_many_allocations( void )
{
    #define N 50
    void* ptrs[ N ];

    for ( int i = 0; i < N; i++ )
    {
        ptrs[ i ] = sand_mem_alloc( ( i + 1 ) * 16, __FILE__, __LINE__ );
        TEST_ASSERT_NOT_NULL( ptrs[ i ] );
    }

    // Free all
    for ( int i = 0; i < N; i++ )
    {
        sand_mem_free( ptrs[ i ], __FILE__, __LINE__ );
    }

    // Allocate again after freeing
    for ( int i = 0; i < N; i++ )
    {
        ptrs[ i ] = sand_mem_alloc( 32, __FILE__, __LINE__ );
        TEST_ASSERT_NOT_NULL( ptrs[ i ] );
    }

    for ( int i = 0; i < N; i++ )
    {
        sand_mem_free( ptrs[ i ], __FILE__, __LINE__ );
    }
    #undef N
}

// =============================================================================
// Main
// =============================================================================

int main( void )
{
    UNITY_BEGIN();

    // alloc
    RUN_TEST( test_chk_alloc_returns_non_null );
    RUN_TEST( test_chk_alloc_returns_aligned_pointer );
    RUN_TEST( test_chk_alloc_memory_is_writable );
    RUN_TEST( test_chk_alloc_various_sizes );
    RUN_TEST( test_chk_alloc_multiple_do_not_overlap );

    // calloc
    RUN_TEST( test_chk_calloc_returns_zeroed );
    RUN_TEST( test_chk_calloc_count_times_size );

    // free
    RUN_TEST( test_chk_free_and_realloc );
    RUN_TEST( test_chk_free_null_is_noop );

    // resize
    RUN_TEST( test_chk_resize_preserves_data );
    RUN_TEST( test_chk_resize_shrink );
    RUN_TEST( test_chk_resize_result_is_aligned );

    // stress
    RUN_TEST( test_chk_many_allocations );

    return UNITY_END();
}
