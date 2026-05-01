#include "unity.h"

#include <string.h>

#include "../Sand_string.h"

static Sand_string_t s;

void setUp( void )
{
    sand_string_create( &s );
}

void tearDown( void )
{
    sand_string_destroy( &s );
}

// =============================================================================
// sand_string_create / sand_string_destroy
// =============================================================================

void test_create_initializes_to_zero( void )
{
    TEST_ASSERT_NULL( s.data );
    TEST_ASSERT_EQUAL_INT32( 0, s.size );
    TEST_ASSERT_EQUAL_INT32( 0, s.capacity );
}

void test_destroy_resets_to_zero( void )
{
    sand_string_append( &s, "hello" );
    sand_string_destroy( &s );

    TEST_ASSERT_NULL( s.data );
    TEST_ASSERT_EQUAL_INT32( 0, s.size );
    TEST_ASSERT_EQUAL_INT32( 0, s.capacity );
}

// =============================================================================
// sand_string_append
// =============================================================================

void test_append_single_string( void )
{
    sand_string_append( &s, "hello" );

    TEST_ASSERT_EQUAL_INT32( 5, s.size );
    TEST_ASSERT_EQUAL_STRING( "hello", s.data );
    TEST_ASSERT_TRUE( s.capacity >= s.size );
}

void test_append_empty_string( void )
{
    sand_string_append( &s, "" );

    TEST_ASSERT_EQUAL_INT32( 0, s.size );
}

void test_append_grows_capacity( void )
{
    sand_string_append( &s, "hello" );
    int32_t cap_after_first = s.capacity;

    // Append enough to force a grow
    sand_string_append( &s, " world, this is a longer string to trigger growth" );

    TEST_ASSERT_TRUE( s.capacity >= cap_after_first );
    TEST_ASSERT_EQUAL_INT32(
        5 + (int32_t)strlen( " world, this is a longer string to trigger growth" ),
        s.size );
}

// =============================================================================
// sand_string_append_n
// =============================================================================

void test_append_n_partial_string( void )
{
    const char* source = "hello world";

    sand_string_append_n( &s, source, 5 );

    TEST_ASSERT_EQUAL_INT32( 5, s.size );
    TEST_ASSERT_EQUAL_INT( 0, memcmp( s.data, "hello", 5 ) );
}

// =============================================================================
// sand_string_left_trim
// =============================================================================

void test_left_trim_whitespace( void )
{
    sand_string_append( &s, "   hello" );
    sand_string_left_trim( &s, 0 );

    TEST_ASSERT_EQUAL_STRING( "hello", s.data );
    TEST_ASSERT_EQUAL_INT32( 5, s.size );
}

void test_left_trim_specific_char( void )
{
    sand_string_append( &s, "xxxhello" );
    sand_string_left_trim( &s, 'x' );

    TEST_ASSERT_EQUAL_STRING( "hello", s.data );
    TEST_ASSERT_EQUAL_INT32( 5, s.size );
}

void test_left_trim_all_whitespace( void )
{
    sand_string_append( &s, "     " );
    sand_string_left_trim( &s, 0 );

    TEST_ASSERT_EQUAL_INT32( 0, s.size );
}

void test_left_trim_no_match( void )
{
    sand_string_append( &s, "hello" );
    sand_string_left_trim( &s, 0 );

    TEST_ASSERT_EQUAL_STRING( "hello", s.data );
    TEST_ASSERT_EQUAL_INT32( 5, s.size );
}

void test_left_trim_tabs_and_spaces( void )
{
    sand_string_append( &s, " \t\n hello" );
    sand_string_left_trim( &s, 0 );

    TEST_ASSERT_EQUAL_STRING( "hello", s.data );
}

// =============================================================================
// sand_string_right_trim
// =============================================================================

void test_right_trim_whitespace( void )
{
    sand_string_append( &s, "hello   " );
    sand_string_right_trim( &s, 0 );

    TEST_ASSERT_EQUAL_STRING( "hello", s.data );
    TEST_ASSERT_EQUAL_INT32( 5, s.size );
}

void test_right_trim_specific_char( void )
{
    sand_string_append( &s, "helloxxx" );
    sand_string_right_trim( &s, 'x' );

    TEST_ASSERT_EQUAL_STRING( "hello", s.data );
    TEST_ASSERT_EQUAL_INT32( 5, s.size );
}

void test_right_trim_all_whitespace( void )
{
    sand_string_append( &s, "     " );
    sand_string_right_trim( &s, 0 );

    TEST_ASSERT_EQUAL_INT32( 0, s.size );
}

void test_right_trim_no_match( void )
{
    sand_string_append( &s, "hello" );
    sand_string_right_trim( &s, 0 );

    TEST_ASSERT_EQUAL_STRING( "hello", s.data );
    TEST_ASSERT_EQUAL_INT32( 5, s.size );
}

// =============================================================================
// sand_string_trim (both sides)
// =============================================================================

void test_trim_both_sides_whitespace( void )
{
    sand_string_append( &s, "   hello   " );
    sand_string_trim( &s, 0 );

    TEST_ASSERT_EQUAL_STRING( "hello", s.data );
    TEST_ASSERT_EQUAL_INT32( 5, s.size );
}

void test_trim_both_sides_specific_char( void )
{
    sand_string_append( &s, "xxxhelloxxx" );
    sand_string_trim( &s, 'x' );

    TEST_ASSERT_EQUAL_STRING( "hello", s.data );
    TEST_ASSERT_EQUAL_INT32( 5, s.size );
}

void test_trim_all_whitespace( void )
{
    sand_string_append( &s, "   \t\n   " );
    sand_string_trim( &s, 0 );

    TEST_ASSERT_EQUAL_INT32( 0, s.size );
}

// =============================================================================
// C-string trim variants
// =============================================================================

void test_left_trim_cstr_whitespace( void )
{
    char buf[] = "   hello";
    sand_string_left_trim_cstr( buf, 0 );

    TEST_ASSERT_EQUAL_STRING( "hello", buf );
}

void test_left_trim_cstr_specific_char( void )
{
    char buf[] = "---hello";
    sand_string_left_trim_cstr( buf, '-' );

    TEST_ASSERT_EQUAL_STRING( "hello", buf );
}

void test_left_trim_cstr_empty( void )
{
    char buf[] = "";
    sand_string_left_trim_cstr( buf, 0 );

    TEST_ASSERT_EQUAL_STRING( "", buf );
}

void test_left_trim_cstr_all_whitespace( void )
{
    char buf[] = "   ";
    sand_string_left_trim_cstr( buf, 0 );

    TEST_ASSERT_EQUAL_STRING( "", buf );
}

void test_right_trim_cstr_whitespace( void )
{
    char buf[] = "hello   ";
    sand_string_right_trim_cstr( buf, 0 );

    TEST_ASSERT_EQUAL_STRING( "hello", buf );
}

void test_right_trim_cstr_specific_char( void )
{
    char buf[] = "hello---";
    sand_string_right_trim_cstr( buf, '-' );

    TEST_ASSERT_EQUAL_STRING( "hello", buf );
}

void test_right_trim_cstr_empty( void )
{
    char buf[] = "";
    sand_string_right_trim_cstr( buf, 0 );

    TEST_ASSERT_EQUAL_STRING( "", buf );
}

void test_trim_cstr_both_sides( void )
{
    char buf[] = "   hello   ";
    sand_string_trim_cstr( buf, 0 );

    TEST_ASSERT_EQUAL_STRING( "hello", buf );
}

void test_trim_cstr_specific_char( void )
{
    char buf[] = "xxhelloxx";
    sand_string_trim_cstr( buf, 'x' );

    TEST_ASSERT_EQUAL_STRING( "hello", buf );
}

// =============================================================================
// Main
// =============================================================================

int main( void )
{
    UNITY_BEGIN();

    // create / destroy
    RUN_TEST( test_create_initializes_to_zero );
    RUN_TEST( test_destroy_resets_to_zero );

    // append
    RUN_TEST( test_append_single_string );
    RUN_TEST( test_append_empty_string );
    RUN_TEST( test_append_grows_capacity );

    // append_n
    RUN_TEST( test_append_n_partial_string );

    // left trim
    RUN_TEST( test_left_trim_whitespace );
    RUN_TEST( test_left_trim_specific_char );
    RUN_TEST( test_left_trim_all_whitespace );
    RUN_TEST( test_left_trim_no_match );
    RUN_TEST( test_left_trim_tabs_and_spaces );

    // right trim
    RUN_TEST( test_right_trim_whitespace );
    RUN_TEST( test_right_trim_specific_char );
    RUN_TEST( test_right_trim_all_whitespace );
    RUN_TEST( test_right_trim_no_match );

    // trim both sides
    RUN_TEST( test_trim_both_sides_whitespace );
    RUN_TEST( test_trim_both_sides_specific_char );
    RUN_TEST( test_trim_all_whitespace );

    // cstr variants
    RUN_TEST( test_left_trim_cstr_whitespace );
    RUN_TEST( test_left_trim_cstr_specific_char );
    RUN_TEST( test_left_trim_cstr_empty );
    RUN_TEST( test_left_trim_cstr_all_whitespace );
    RUN_TEST( test_right_trim_cstr_whitespace );
    RUN_TEST( test_right_trim_cstr_specific_char );
    RUN_TEST( test_right_trim_cstr_empty );
    RUN_TEST( test_trim_cstr_both_sides );
    RUN_TEST( test_trim_cstr_specific_char );

    return UNITY_END();
}
