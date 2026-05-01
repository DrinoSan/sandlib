#include "unity.h"

#include "../Sand_vector.h"

static sand_list* list;

void setUp( void )
{
    list = NULL;
}

void tearDown( void )
{
    if ( list )
    {
        sand_list_free( list );
        free( list );
        list = NULL;
    }
}

// =============================================================================
// sand_list_create
// =============================================================================

void test_create_int_list( void )
{
    list = sand_list_create( sizeof( int ), NULL );

    TEST_ASSERT_NOT_NULL( list );
    TEST_ASSERT_EQUAL_size_t( 0, list->size );
    TEST_ASSERT_EQUAL_size_t( 0, list->capacity );
    TEST_ASSERT_EQUAL_size_t( sizeof( int ), list->type_size );
    TEST_ASSERT_NULL( list->freeElem );
}

void test_create_double_list( void )
{
    list = sand_list_create( sizeof( double ), NULL );

    TEST_ASSERT_NOT_NULL( list );
    TEST_ASSERT_EQUAL_size_t( sizeof( double ), list->type_size );
}

// =============================================================================
// sand_list_push_back
// =============================================================================

void test_push_back_single_int( void )
{
    list  = sand_list_create( sizeof( int ), NULL );
    int v = 42;

    sand_list_push_back( list, &v );

    TEST_ASSERT_EQUAL_size_t( 1, list->size );
    TEST_ASSERT_TRUE( list->capacity >= 1 );
}

void test_push_back_multiple_ints( void )
{
    list = sand_list_create( sizeof( int ), NULL );

    for ( int i = 0; i < 10; i++ )
    {
        sand_list_push_back( list, &i );
    }

    TEST_ASSERT_EQUAL_size_t( 10, list->size );
    TEST_ASSERT_TRUE( list->capacity >= 10 );

    // Verify all values
    for ( int i = 0; i < 10; i++ )
    {
        int val;
        int ok = sand_list_get( list, i, &val );
        TEST_ASSERT_EQUAL_INT( 1, ok );
        TEST_ASSERT_EQUAL_INT( i, val );
    }
}

void test_push_back_triggers_growth( void )
{
    list = sand_list_create( sizeof( int ), NULL );

    // Initial capacity is 4 after first push, push 5 elements to force growth
    for ( int i = 0; i < 5; i++ )
    {
        sand_list_push_back( list, &i );
    }

    TEST_ASSERT_EQUAL_size_t( 5, list->size );
    TEST_ASSERT_TRUE( list->capacity >= 5 );

    // Verify integrity after growth
    int val;
    sand_list_get( list, 0, &val );
    TEST_ASSERT_EQUAL_INT( 0, val );

    sand_list_get( list, 4, &val );
    TEST_ASSERT_EQUAL_INT( 4, val );
}

void test_push_back_returns_list_pointer( void )
{
    list  = sand_list_create( sizeof( int ), NULL );
    int v = 1;

    sand_list* ret = sand_list_push_back( list, &v );

    TEST_ASSERT_EQUAL_PTR( list, ret );
}

// =============================================================================
// sand_list_pop_back
// =============================================================================

void test_pop_back_returns_last_element( void )
{
    list = sand_list_create( sizeof( int ), NULL );
    int values[] = { 10, 20, 30 };

    for ( int i = 0; i < 3; i++ )
    {
        sand_list_push_back( list, &values[ i ] );
    }

    int out;
    int ok = sand_list_pop_back( list, &out );

    TEST_ASSERT_EQUAL_INT( 1, ok );
    TEST_ASSERT_EQUAL_INT( 30, out );
    TEST_ASSERT_EQUAL_size_t( 2, list->size );
}

void test_pop_back_empty_list_returns_zero( void )
{
    list = sand_list_create( sizeof( int ), NULL );

    int out;
    int ok = sand_list_pop_back( list, &out );

    TEST_ASSERT_EQUAL_INT( 0, ok );
}

void test_pop_back_null_out_just_decrements( void )
{
    list  = sand_list_create( sizeof( int ), NULL );
    int v = 99;
    sand_list_push_back( list, &v );

    int ok = sand_list_pop_back( list, NULL );

    TEST_ASSERT_EQUAL_INT( 1, ok );
    TEST_ASSERT_EQUAL_size_t( 0, list->size );
}

// =============================================================================
// sand_list_get
// =============================================================================

void test_get_valid_index( void )
{
    list = sand_list_create( sizeof( int ), NULL );
    int values[] = { 100, 200, 300 };

    for ( int i = 0; i < 3; i++ )
    {
        sand_list_push_back( list, &values[ i ] );
    }

    int out;
    TEST_ASSERT_EQUAL_INT( 1, sand_list_get( list, 0, &out ) );
    TEST_ASSERT_EQUAL_INT( 100, out );

    TEST_ASSERT_EQUAL_INT( 1, sand_list_get( list, 1, &out ) );
    TEST_ASSERT_EQUAL_INT( 200, out );

    TEST_ASSERT_EQUAL_INT( 1, sand_list_get( list, 2, &out ) );
    TEST_ASSERT_EQUAL_INT( 300, out );
}

void test_get_out_of_bounds( void )
{
    list  = sand_list_create( sizeof( int ), NULL );
    int v = 1;
    sand_list_push_back( list, &v );

    int out;
    int ok = sand_list_get( list, 5, &out );

    TEST_ASSERT_EQUAL_INT( 0, ok );
}

// =============================================================================
// sand_list_at
// =============================================================================

void test_at_valid_index( void )
{
    list = sand_list_create( sizeof( int ), NULL );
    int values[] = { 10, 20, 30 };

    for ( int i = 0; i < 3; i++ )
    {
        sand_list_push_back( list, &values[ i ] );
    }

    int* p = (int*)sand_list_at( list, 1 );
    TEST_ASSERT_NOT_NULL( p );
    TEST_ASSERT_EQUAL_INT( 20, *p );
}

void test_at_out_of_bounds( void )
{
    list  = sand_list_create( sizeof( int ), NULL );
    int v = 1;
    sand_list_push_back( list, &v );

    void* p = sand_list_at( list, 99 );

    TEST_ASSERT_NULL( p );
}

void test_at_allows_in_place_modification( void )
{
    list = sand_list_create( sizeof( int ), NULL );
    int v = 5;
    sand_list_push_back( list, &v );

    int* p = (int*)sand_list_at( list, 0 );
    *p     = 999;

    int out;
    sand_list_get( list, 0, &out );
    TEST_ASSERT_EQUAL_INT( 999, out );
}

// =============================================================================
// sand_list_clear
// =============================================================================

void test_clear_resets_size( void )
{
    list = sand_list_create( sizeof( int ), NULL );

    for ( int i = 0; i < 5; i++ )
    {
        sand_list_push_back( list, &i );
    }

    sand_list_clear( list );

    TEST_ASSERT_EQUAL_size_t( 0, list->size );
    TEST_ASSERT_TRUE( list->capacity > 0 );   // capacity preserved
}

void test_clear_null_returns_null( void )
{
    sand_list* ret = sand_list_clear( NULL );

    TEST_ASSERT_NULL( ret );
}

// =============================================================================
// sand_list_shrink_to_fit
// =============================================================================

void test_shrink_to_fit_reduces_capacity( void )
{
    list = sand_list_create( sizeof( int ), NULL );

    for ( int i = 0; i < 10; i++ )
    {
        sand_list_push_back( list, &i );
    }

    // Pop some elements to create slack
    sand_list_pop_back( list, NULL );
    sand_list_pop_back( list, NULL );
    sand_list_pop_back( list, NULL );

    TEST_ASSERT_TRUE( list->capacity > list->size );

    sand_list_shrink_to_fit( list );

    TEST_ASSERT_EQUAL_size_t( list->size, list->capacity );
}

void test_shrink_already_fit_is_noop( void )
{
    list  = sand_list_create( sizeof( int ), NULL );
    int v = 1;
    sand_list_push_back( list, &v );
    sand_list_shrink_to_fit( list );   // capacity = 4, size = 1

    // After shrink, capacity should equal size
    sand_list_shrink_to_fit( list );
    TEST_ASSERT_EQUAL_size_t( list->size, list->capacity );
}

// =============================================================================
// Type variety: doubles
// =============================================================================

void test_push_and_get_doubles( void )
{
    list = sand_list_create( sizeof( double ), NULL );

    double values[] = { 3.14, 2.71, 1.41 };

    for ( int i = 0; i < 3; i++ )
    {
        sand_list_push_back( list, &values[ i ] );
    }

    double out;
    sand_list_get( list, 0, &out );
    TEST_ASSERT_EQUAL_DOUBLE( 3.14, out );

    sand_list_get( list, 2, &out );
    TEST_ASSERT_EQUAL_DOUBLE( 1.41, out );
}

// =============================================================================
// Type variety: structs
// =============================================================================

typedef struct
{
    int x;
    int y;
} point_t;

void test_push_and_get_structs( void )
{
    list = sand_list_create( sizeof( point_t ), NULL );

    point_t p1 = { .x = 1, .y = 2 };
    point_t p2 = { .x = 3, .y = 4 };

    sand_list_push_back( list, &p1 );
    sand_list_push_back( list, &p2 );

    point_t out;
    sand_list_get( list, 1, &out );
    TEST_ASSERT_EQUAL_INT( 3, out.x );
    TEST_ASSERT_EQUAL_INT( 4, out.y );
}

// =============================================================================
// Main
// =============================================================================

int main( void )
{
    UNITY_BEGIN();

    // create
    RUN_TEST( test_create_int_list );
    RUN_TEST( test_create_double_list );

    // push_back
    RUN_TEST( test_push_back_single_int );
    RUN_TEST( test_push_back_multiple_ints );
    RUN_TEST( test_push_back_triggers_growth );
    RUN_TEST( test_push_back_returns_list_pointer );

    // pop_back
    RUN_TEST( test_pop_back_returns_last_element );
    RUN_TEST( test_pop_back_empty_list_returns_zero );
    RUN_TEST( test_pop_back_null_out_just_decrements );

    // get
    RUN_TEST( test_get_valid_index );
    RUN_TEST( test_get_out_of_bounds );

    // at
    RUN_TEST( test_at_valid_index );
    RUN_TEST( test_at_out_of_bounds );
    RUN_TEST( test_at_allows_in_place_modification );

    // clear
    RUN_TEST( test_clear_resets_size );
    RUN_TEST( test_clear_null_returns_null );

    // shrink_to_fit
    RUN_TEST( test_shrink_to_fit_reduces_capacity );
    RUN_TEST( test_shrink_already_fit_is_noop );

    // type variety
    RUN_TEST( test_push_and_get_doubles );
    RUN_TEST( test_push_and_get_structs );

    return UNITY_END();
}
