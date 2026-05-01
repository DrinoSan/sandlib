#include "unity.h"

#include <string.h>

#include "../Sand_string_view.h"

void setUp( void )
{
}

void tearDown( void )
{
}

// =============================================================================
// sand_string_view_left_trim
// =============================================================================

void test_sv_left_trim_whitespace( void )
{
    char                buf[] = "   hello";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    sand_string_view_left_trim( &sv, 0 );

    TEST_ASSERT_EQUAL_size_t( 5, sv.size );
    TEST_ASSERT_EQUAL_INT( 0, memcmp( sv.data, "hello", 5 ) );
}

void test_sv_left_trim_specific_char( void )
{
    char                buf[] = "xxxhello";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    sand_string_view_left_trim( &sv, 'x' );

    TEST_ASSERT_EQUAL_size_t( 5, sv.size );
    TEST_ASSERT_EQUAL_INT( 0, memcmp( sv.data, "hello", 5 ) );
}

void test_sv_left_trim_all_whitespace( void )
{
    char                buf[] = "     ";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    sand_string_view_left_trim( &sv, 0 );

    TEST_ASSERT_NULL( sv.data );
    TEST_ASSERT_EQUAL_size_t( 0, sv.size );
}

void test_sv_left_trim_no_match( void )
{
    char                buf[] = "hello";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    sand_string_view_left_trim( &sv, 0 );

    TEST_ASSERT_EQUAL_size_t( 5, sv.size );
    TEST_ASSERT_EQUAL_INT( 0, memcmp( sv.data, "hello", 5 ) );
}

void test_sv_left_trim_empty( void )
{
    sand_string_view_t sv = { .data = NULL, .size = 0 };

    sand_string_view_left_trim( &sv, 0 );

    TEST_ASSERT_EQUAL_size_t( 0, sv.size );
}

// =============================================================================
// sand_string_view_right_trim
// =============================================================================

void test_sv_right_trim_whitespace( void )
{
    char                buf[] = "hello   ";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    sand_string_view_right_trim( &sv, 0 );

    TEST_ASSERT_EQUAL_size_t( 5, sv.size );
    TEST_ASSERT_EQUAL_INT( 0, memcmp( sv.data, "hello", 5 ) );
}

void test_sv_right_trim_specific_char( void )
{
    char                buf[] = "helloxxx";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    sand_string_view_right_trim( &sv, 'x' );

    TEST_ASSERT_EQUAL_size_t( 5, sv.size );
    TEST_ASSERT_EQUAL_INT( 0, memcmp( sv.data, "hello", 5 ) );
}

void test_sv_right_trim_all_whitespace( void )
{
    char                buf[] = "     ";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    sand_string_view_right_trim( &sv, 0 );

    TEST_ASSERT_NULL( sv.data );
    TEST_ASSERT_EQUAL_size_t( 0, sv.size );
}

void test_sv_right_trim_no_match( void )
{
    char                buf[] = "hello";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    sand_string_view_right_trim( &sv, 0 );

    TEST_ASSERT_EQUAL_size_t( 5, sv.size );
    TEST_ASSERT_EQUAL_INT( 0, memcmp( sv.data, "hello", 5 ) );
}

void test_sv_right_trim_empty( void )
{
    sand_string_view_t sv = { .data = NULL, .size = 0 };

    sand_string_view_right_trim( &sv, 0 );

    TEST_ASSERT_EQUAL_size_t( 0, sv.size );
}

// =============================================================================
// sand_string_view_trim (both sides)
// =============================================================================

void test_sv_trim_both_sides_whitespace( void )
{
    char                buf[] = "   hello   ";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    sand_string_view_trim( &sv, 0 );

    TEST_ASSERT_EQUAL_size_t( 5, sv.size );
    TEST_ASSERT_EQUAL_INT( 0, memcmp( sv.data, "hello", 5 ) );
}

void test_sv_trim_both_sides_specific_char( void )
{
    char                buf[] = "xxhelloxx";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    sand_string_view_trim( &sv, 'x' );

    TEST_ASSERT_EQUAL_size_t( 5, sv.size );
    TEST_ASSERT_EQUAL_INT( 0, memcmp( sv.data, "hello", 5 ) );
}

void test_sv_trim_all_whitespace( void )
{
    char                buf[] = "   \t\n   ";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    sand_string_view_trim( &sv, 0 );

    TEST_ASSERT_NULL( sv.data );
    TEST_ASSERT_EQUAL_size_t( 0, sv.size );
}

void test_sv_trim_single_char( void )
{
    char                buf[] = "  a  ";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    sand_string_view_trim( &sv, 0 );

    TEST_ASSERT_EQUAL_size_t( 1, sv.size );
    TEST_ASSERT_EQUAL_CHAR( 'a', sv.data[ 0 ] );
}

void test_sv_does_not_own_data( void )
{
    // Verify that the view operates on the original buffer
    char                buf[] = "   hello   ";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    sand_string_view_left_trim( &sv, 0 );

    // sv.data should point into buf (not a copy)
    TEST_ASSERT_TRUE( sv.data >= buf && sv.data < buf + sizeof( buf ) );
}

// =============================================================================
// Main
// =============================================================================

int main( void )
{
    UNITY_BEGIN();

    // left trim
    RUN_TEST( test_sv_left_trim_whitespace );
    RUN_TEST( test_sv_left_trim_specific_char );
    RUN_TEST( test_sv_left_trim_all_whitespace );
    RUN_TEST( test_sv_left_trim_no_match );
    RUN_TEST( test_sv_left_trim_empty );

    // right trim
    RUN_TEST( test_sv_right_trim_whitespace );
    RUN_TEST( test_sv_right_trim_specific_char );
    RUN_TEST( test_sv_right_trim_all_whitespace );
    RUN_TEST( test_sv_right_trim_no_match );
    RUN_TEST( test_sv_right_trim_empty );

    // trim both
    RUN_TEST( test_sv_trim_both_sides_whitespace );
    RUN_TEST( test_sv_trim_both_sides_specific_char );
    RUN_TEST( test_sv_trim_all_whitespace );
    RUN_TEST( test_sv_trim_single_char );
    RUN_TEST( test_sv_does_not_own_data );

    return UNITY_END();
}
