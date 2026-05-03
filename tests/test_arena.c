#include "unity.h"

#include <stdint.h>
#include <string.h>

#include "../arena.h"
#include "../except.h"

// Mirror the alignment union from arena.c to verify alignment
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
    sand_except_stack = NULL;
}

void tearDown( void )
{
    sand_except_stack = NULL;
}

// =============================================================================
// sand_arena_new
// =============================================================================

void test_arena_new_returns_non_null( void )
{
    sand_arena_t* arena = sand_arena_new();

    TEST_ASSERT_NOT_NULL( arena );

    sand_arena_destroy( &arena );
}

void test_arena_new_destroy_sets_null( void )
{
    sand_arena_t* arena = sand_arena_new();

    sand_arena_destroy( &arena );

    TEST_ASSERT_NULL( arena );
}

// =============================================================================
// sand_arena_alloc
// =============================================================================

void test_arena_alloc_returns_non_null( void )
{
    sand_arena_t* arena = sand_arena_new();

    void* p = sand_arena_alloc( arena, 100, __FILE__, __LINE__ );

    TEST_ASSERT_NOT_NULL( p );

    sand_arena_destroy( &arena );
}

void test_arena_alloc_returns_aligned_pointer( void )
{
    sand_arena_t* arena = sand_arena_new();

    void* p = sand_arena_alloc( arena, 1, __FILE__, __LINE__ );

    TEST_ASSERT_NOT_NULL( p );
    TEST_ASSERT_EQUAL_UINT64(
        0, ( uintptr_t ) p % sizeof( union align ) );

    sand_arena_destroy( &arena );
}

void test_arena_alloc_memory_is_writable( void )
{
    sand_arena_t* arena = sand_arena_new();

    char* p = sand_arena_alloc( arena, 64, __FILE__, __LINE__ );

    memset( p, 'X', 64 );
    TEST_ASSERT_EQUAL_CHAR( 'X', p[ 0 ] );
    TEST_ASSERT_EQUAL_CHAR( 'X', p[ 63 ] );

    sand_arena_destroy( &arena );
}

void test_arena_alloc_various_sizes( void )
{
    sand_arena_t* arena = sand_arena_new();

    void* p1 = sand_arena_alloc( arena, 1, __FILE__, __LINE__ );
    void* p2 = sand_arena_alloc( arena, 100, __FILE__, __LINE__ );
    void* p3 = sand_arena_alloc( arena, 4096, __FILE__, __LINE__ );

    TEST_ASSERT_NOT_NULL( p1 );
    TEST_ASSERT_NOT_NULL( p2 );
    TEST_ASSERT_NOT_NULL( p3 );

    TEST_ASSERT_EQUAL_UINT64(
        0, ( uintptr_t ) p1 % sizeof( union align ) );
    TEST_ASSERT_EQUAL_UINT64(
        0, ( uintptr_t ) p2 % sizeof( union align ) );
    TEST_ASSERT_EQUAL_UINT64(
        0, ( uintptr_t ) p3 % sizeof( union align ) );

    sand_arena_destroy( &arena );
}

void test_arena_alloc_multiple_do_not_overlap( void )
{
    sand_arena_t* arena = sand_arena_new();

    char* a = sand_arena_alloc( arena, 64, __FILE__, __LINE__ );
    char* b = sand_arena_alloc( arena, 64, __FILE__, __LINE__ );

    memset( a, 'A', 64 );
    memset( b, 'B', 64 );

    for ( int i = 0; i < 64; i++ )
    {
        TEST_ASSERT_EQUAL_CHAR( 'A', a[ i ] );
        TEST_ASSERT_EQUAL_CHAR( 'B', b[ i ] );
    }

    sand_arena_destroy( &arena );
}

void test_arena_alloc_sequential_addresses_increase( void )
{
    sand_arena_t* arena = sand_arena_new();

    char* p1 = sand_arena_alloc( arena, 16, __FILE__, __LINE__ );
    char* p2 = sand_arena_alloc( arena, 16, __FILE__, __LINE__ );
    char* p3 = sand_arena_alloc( arena, 16, __FILE__, __LINE__ );

    // bump allocator: addresses go forward within the same chunk
    TEST_ASSERT_TRUE( p2 > p1 );
    TEST_ASSERT_TRUE( p3 > p2 );

    sand_arena_destroy( &arena );
}

void test_arena_alloc_large_triggers_new_chunk( void )
{
    sand_arena_t* arena = sand_arena_new();

    // First alloc creates a chunk with ~10KB slack
    char* p1 = sand_arena_alloc( arena, 16, __FILE__, __LINE__ );
    memset( p1, 'A', 16 );

    // This should exceed the slack and trigger a new chunk
    char* p2 = sand_arena_alloc( arena, 20 * 1024, __FILE__, __LINE__ );
    memset( p2, 'B', 20 * 1024 );

    // Both should still be valid and non-overlapping
    TEST_ASSERT_EQUAL_CHAR( 'A', p1[ 0 ] );
    TEST_ASSERT_EQUAL_CHAR( 'B', p2[ 0 ] );
    TEST_ASSERT_EQUAL_CHAR( 'B', p2[ 20 * 1024 - 1 ] );

    sand_arena_destroy( &arena );
}

void test_arena_alloc_preserves_previous_data( void )
{
    sand_arena_t* arena = sand_arena_new();

    char* p1 = sand_arena_alloc( arena, 32, __FILE__, __LINE__ );
    memcpy( p1, "hello world", 12 );

    // Allocate more to potentially trigger new chunks
    for ( int i = 0; i < 20; i++ )
    {
        char* p = sand_arena_alloc( arena, 512, __FILE__, __LINE__ );
        memset( p, 'Z', 512 );
    }

    // First allocation should still be intact
    TEST_ASSERT_EQUAL_INT( 0, memcmp( p1, "hello world", 12 ) );

    sand_arena_destroy( &arena );
}

// =============================================================================
// sand_arena_calloc
// =============================================================================

void test_arena_calloc_returns_zeroed( void )
{
    sand_arena_t* arena = sand_arena_new();

    unsigned char* p = sand_arena_calloc( arena, 1, 128, __FILE__, __LINE__ );

    TEST_ASSERT_NOT_NULL( p );

    for ( int i = 0; i < 128; i++ )
    {
        TEST_ASSERT_EQUAL_UINT8( 0, p[ i ] );
    }

    sand_arena_destroy( &arena );
}

void test_arena_calloc_count_times_size( void )
{
    sand_arena_t* arena = sand_arena_new();

    int* p = sand_arena_calloc( arena, 10, sizeof( int ), __FILE__, __LINE__ );

    TEST_ASSERT_NOT_NULL( p );

    for ( int i = 0; i < 10; i++ )
    {
        TEST_ASSERT_EQUAL_INT( 0, p[ i ] );
    }

    sand_arena_destroy( &arena );
}

void test_arena_calloc_is_aligned( void )
{
    sand_arena_t* arena = sand_arena_new();

    void* p = sand_arena_calloc( arena, 5, 7, __FILE__, __LINE__ );

    TEST_ASSERT_EQUAL_UINT64(
        0, ( uintptr_t ) p % sizeof( union align ) );

    sand_arena_destroy( &arena );
}

// =============================================================================
// sand_arena_free
// =============================================================================

void test_arena_free_allows_reuse( void )
{
    sand_arena_t* arena = sand_arena_new();

    char* p1 = sand_arena_alloc( arena, 64, __FILE__, __LINE__ );
    memcpy( p1, "before free", 12 );

    sand_arena_free( arena );

    // Arena is reset, allocate again
    char* p2 = sand_arena_alloc( arena, 64, __FILE__, __LINE__ );
    memcpy( p2, "after free", 11 );

    TEST_ASSERT_NOT_NULL( p2 );
    TEST_ASSERT_EQUAL_INT( 0, memcmp( p2, "after free", 11 ) );

    sand_arena_destroy( &arena );
}

void test_arena_free_multiple_chunks( void )
{
    sand_arena_t* arena = sand_arena_new();

    // Allocate enough to create multiple chunks
    for ( int i = 0; i < 20; i++ )
    {
        char* p = sand_arena_alloc( arena, 1024, __FILE__, __LINE__ );
        TEST_ASSERT_NOT_NULL( p );
        memset( p, ( char ) i, 1024 );
    }

    // Free all chunks
    sand_arena_free( arena );

    // Should be able to allocate again
    char* p = sand_arena_alloc( arena, 32, __FILE__, __LINE__ );
    TEST_ASSERT_NOT_NULL( p );

    sand_arena_destroy( &arena );
}

void test_arena_free_then_alloc_reuses_chunks( void )
{
    sand_arena_t* arena = sand_arena_new();

    // Create some chunks
    for ( int i = 0; i < 5; i++ )
    {
        sand_arena_alloc( arena, 2048, __FILE__, __LINE__ );
    }

    // Free puts chunks on freechunks list
    sand_arena_free( arena );

    // New allocations should reuse cached chunks (no malloc needed)
    for ( int i = 0; i < 5; i++ )
    {
        void* p = sand_arena_alloc( arena, 2048, __FILE__, __LINE__ );
        TEST_ASSERT_NOT_NULL( p );
    }

    sand_arena_destroy( &arena );
}

// =============================================================================
// sand_arena_destroy
// =============================================================================

void test_arena_destroy_after_allocations( void )
{
    sand_arena_t* arena = sand_arena_new();

    for ( int i = 0; i < 10; i++ )
    {
        void* p = sand_arena_alloc( arena, 256, __FILE__, __LINE__ );
        TEST_ASSERT_NOT_NULL( p );
    }

    sand_arena_destroy( &arena );

    TEST_ASSERT_NULL( arena );
}

void test_arena_destroy_empty_arena( void )
{
    sand_arena_t* arena = sand_arena_new();

    // Destroy without any allocations
    sand_arena_destroy( &arena );

    TEST_ASSERT_NULL( arena );
}

// =============================================================================
// Stress tests
// =============================================================================

void test_arena_many_small_allocations( void )
{
    sand_arena_t* arena = sand_arena_new();

    #define N 500
    void* ptrs[ N ];

    for ( int i = 0; i < N; i++ )
    {
        ptrs[ i ] = sand_arena_alloc( arena, 16, __FILE__, __LINE__ );
        TEST_ASSERT_NOT_NULL( ptrs[ i ] );
        TEST_ASSERT_EQUAL_UINT64(
            0, ( uintptr_t ) ptrs[ i ] % sizeof( union align ) );
    }

    // All allocations should still be valid
    for ( int i = 0; i < N; i++ )
    {
        memset( ptrs[ i ], 'A', 16 );
    }

    for ( int i = 0; i < N; i++ )
    {
        char* p = ptrs[ i ];
        TEST_ASSERT_EQUAL_CHAR( 'A', p[ 0 ] );
        TEST_ASSERT_EQUAL_CHAR( 'A', p[ 15 ] );
    }

    sand_arena_destroy( &arena );
    #undef N
}

void test_arena_free_and_realloc_cycle( void )
{
    sand_arena_t* arena = sand_arena_new();

    for ( int cycle = 0; cycle < 5; cycle++ )
    {
        for ( int i = 0; i < 50; i++ )
        {
            char* p = sand_arena_alloc( arena, 128, __FILE__, __LINE__ );
            TEST_ASSERT_NOT_NULL( p );
            memset( p, ( char ) cycle, 128 );
        }

        sand_arena_free( arena );
    }

    sand_arena_destroy( &arena );
}

void test_arena_mixed_sizes( void )
{
    sand_arena_t* arena = sand_arena_new();

    char* p1 = sand_arena_alloc( arena, 1, __FILE__, __LINE__ );
    char* p2 = sand_arena_alloc( arena, 7, __FILE__, __LINE__ );
    char* p3 = sand_arena_alloc( arena, 256, __FILE__, __LINE__ );
    char* p4 = sand_arena_alloc( arena, 15000, __FILE__, __LINE__ );
    char* p5 = sand_arena_alloc( arena, 3, __FILE__, __LINE__ );

    memset( p1, 'A', 1 );
    memset( p2, 'B', 7 );
    memset( p3, 'C', 256 );
    memset( p4, 'D', 15000 );
    memset( p5, 'E', 3 );

    TEST_ASSERT_EQUAL_CHAR( 'A', p1[ 0 ] );
    TEST_ASSERT_EQUAL_CHAR( 'B', p2[ 6 ] );
    TEST_ASSERT_EQUAL_CHAR( 'C', p3[ 255 ] );
    TEST_ASSERT_EQUAL_CHAR( 'D', p4[ 14999 ] );
    TEST_ASSERT_EQUAL_CHAR( 'E', p5[ 2 ] );

    sand_arena_destroy( &arena );
}

// =============================================================================
// Main
// =============================================================================

int main( void )
{
    UNITY_BEGIN();

    // new
    RUN_TEST( test_arena_new_returns_non_null );
    RUN_TEST( test_arena_new_destroy_sets_null );

    // alloc
    RUN_TEST( test_arena_alloc_returns_non_null );
    RUN_TEST( test_arena_alloc_returns_aligned_pointer );
    RUN_TEST( test_arena_alloc_memory_is_writable );
    RUN_TEST( test_arena_alloc_various_sizes );
    RUN_TEST( test_arena_alloc_multiple_do_not_overlap );
    RUN_TEST( test_arena_alloc_sequential_addresses_increase );
    RUN_TEST( test_arena_alloc_large_triggers_new_chunk );
    RUN_TEST( test_arena_alloc_preserves_previous_data );

    // calloc
    RUN_TEST( test_arena_calloc_returns_zeroed );
    RUN_TEST( test_arena_calloc_count_times_size );
    RUN_TEST( test_arena_calloc_is_aligned );

    // free
    RUN_TEST( test_arena_free_allows_reuse );
    RUN_TEST( test_arena_free_multiple_chunks );
    RUN_TEST( test_arena_free_then_alloc_reuses_chunks );

    // destroy
    RUN_TEST( test_arena_destroy_after_allocations );
    RUN_TEST( test_arena_destroy_empty_arena );

    // stress
    RUN_TEST( test_arena_many_small_allocations );
    RUN_TEST( test_arena_free_and_realloc_cycle );
    RUN_TEST( test_arena_mixed_sizes );

    return UNITY_END();
}
