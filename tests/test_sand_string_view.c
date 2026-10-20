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
// sand_string_view_reset / sand_string_view_set
// =============================================================================

void test_sv_reset( void )
{
    char               buf[] = "hello";
    sand_string_view_t sv    = { .data = buf, .size = 5 };

    sand_string_view_reset( &sv );

    TEST_ASSERT_NULL( sv.data );
    TEST_ASSERT_EQUAL_size_t( 0, sv.size );
}

void test_sv_set( void )
{
    const char*        lit = "hello";
    sand_string_view_t sv;

    sand_string_view_set( &sv, lit );

    TEST_ASSERT_EQUAL_PTR( lit, sv.data );
    TEST_ASSERT_EQUAL_size_t( 5, sv.size );
}

void test_sv_set_empty( void )
{
    const char*        lit = "";
    sand_string_view_t sv;

    sand_string_view_set( &sv, lit );

    TEST_ASSERT_EQUAL_PTR( lit, sv.data );
    TEST_ASSERT_EQUAL_size_t( 0, sv.size );
}

void test_sv_set_null( void )
{
    sand_string_view_t sv;

    sand_string_view_set( &sv, "temp" );
    sand_string_view_set( &sv, NULL );

    TEST_ASSERT_NULL( sv.data );
    TEST_ASSERT_EQUAL_size_t( 0, sv.size );
}

// =============================================================================
// sand_string_view_has_substr
// =============================================================================

void test_sv_substr_positive( void )
{
    char               buf[] = "sandrino";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    TEST_ASSERT_TRUE( sand_string_view_has_substr( &sv, "and" ) );
}

void test_sv_substr_negative( void )
{
    char               buf[] = "sandrino";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    TEST_ASSERT_FALSE( sand_string_view_has_substr( &sv, "andd" ) );
}

void test_sv_substr_too_long( void )
{
    char               buf[] = "sandrino";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    TEST_ASSERT_FALSE( sand_string_view_has_substr( &sv, "sandrino hello" ) );
}

void test_sv_substr_zero_length_needle( void )
{
    char               buf[] = "sandrino";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    TEST_ASSERT_TRUE( sand_string_view_has_substr( &sv, "" ) );
}

void test_sv_substr_empty_view( void )
{
    sand_string_view_t sv = { .data = NULL, .size = 0 };

    TEST_ASSERT_FALSE( sand_string_view_has_substr( &sv, "a" ) );
    TEST_ASSERT_TRUE( sand_string_view_has_substr( &sv, "" ) );
}

void test_sv_substr_unterminated( void )
{
    char               raw[ 5 ] = { 'a', 'b', 'c', 'd', 'e' };
    sand_string_view_t sv       = { .data = raw, .size = 3 };

    TEST_ASSERT_TRUE( sand_string_view_has_substr( &sv, "bc" ) );
    TEST_ASSERT_FALSE( sand_string_view_has_substr( &sv, "cd" ) );
}

// =============================================================================
// sand_string_view_has_substr_no_case
// =============================================================================

void test_sv_substr_nocase_positive( void )
{
    char               buf[] = "SandRino";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    TEST_ASSERT_TRUE( sand_string_view_has_substr_no_case( &sv, "andr" ) );
    TEST_ASSERT_TRUE( sand_string_view_has_substr_no_case( &sv, "SAND" ) );
}

void test_sv_substr_nocase_negative( void )
{
    char               buf[] = "SandRino";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    TEST_ASSERT_FALSE( sand_string_view_has_substr_no_case( &sv, "xyz" ) );
}

void test_sv_substr_nocase_zero_length_needle( void )
{
    char               buf[] = "SandRino";
    sand_string_view_t sv    = { .data = buf, .size = strlen( buf ) };

    TEST_ASSERT_TRUE( sand_string_view_has_substr_no_case( &sv, "" ) );
}

void test_sv_substr_nocase_empty_view( void )
{
    sand_string_view_t sv = { .data = NULL, .size = 0 };

    TEST_ASSERT_FALSE( sand_string_view_has_substr_no_case( &sv, "a" ) );
    TEST_ASSERT_TRUE( sand_string_view_has_substr_no_case( &sv, "" ) );
}

void test_sv_substr_nocase_unterminated( void )
{
    char               raw[ 4 ] = { 'A', 'b', 'C', 'Z' };
    sand_string_view_t sv       = { .data = raw, .size = 3 };

    TEST_ASSERT_TRUE( sand_string_view_has_substr_no_case( &sv, "abc" ) );
    TEST_ASSERT_FALSE( sand_string_view_has_substr_no_case( &sv, "cz" ) );
}

// =============================================================================
// trim edges: non-terminated views, and left_trim past size
// =============================================================================

void test_sv_left_trim_unterminated( void )
{
    char               raw[ 6 ] = { ' ', ' ', 'a', 'b', 'c', 'd' };
    sand_string_view_t sv       = { .data = raw, .size = 5 };

    sand_string_view_left_trim( &sv, 0 );

    TEST_ASSERT_EQUAL_size_t( 3, sv.size );
    TEST_ASSERT_EQUAL_INT( 0, memcmp( sv.data, "abc", 3 ) );
}

void test_sv_right_trim_unterminated( void )
{
    char               raw[ 6 ] = { 'a', 'b', 'c', ' ', ' ', 'x' };
    sand_string_view_t sv       = { .data = raw, .size = 5 };

    sand_string_view_right_trim( &sv, 0 );

    TEST_ASSERT_EQUAL_size_t( 3, sv.size );
    TEST_ASSERT_EQUAL_INT( 0, memcmp( sv.data, "abc", 3 ) );
}

void test_sv_trim_unterminated( void )
{
    char               raw[ 4 ] = { ' ', 'a', ' ', 'x' };
    sand_string_view_t sv       = { .data = raw, .size = 3 };

    sand_string_view_trim( &sv, 0 );

    TEST_ASSERT_EQUAL_size_t( 1, sv.size );
    TEST_ASSERT_EQUAL_CHAR( 'a', sv.data[ 0 ] );
}

void test_sv_left_trim_respects_size( void )
{
    TEST_IGNORE_MESSAGE(
        "Bekannter Bug: Sand_string_view.c:29 left_trim ignoriert size "
        "siehe DrinoSan/Fun docs/sandlib-TODO.md" );

    char               buf[] = "xxhello";
    sand_string_view_t sv    = { .data = buf, .size = 2 };

    sand_string_view_left_trim( &sv, 'x' );

    TEST_ASSERT_NULL( sv.data );
    TEST_ASSERT_EQUAL_size_t( 0, sv.size );
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

    // reset / set
    RUN_TEST( test_sv_reset );
    RUN_TEST( test_sv_set );
    RUN_TEST( test_sv_set_empty );
    RUN_TEST( test_sv_set_null );

    // substr
    RUN_TEST( test_sv_substr_positive );
    RUN_TEST( test_sv_substr_negative );
    RUN_TEST( test_sv_substr_too_long );
    RUN_TEST( test_sv_substr_zero_length_needle );
    RUN_TEST( test_sv_substr_empty_view );
    RUN_TEST( test_sv_substr_unterminated );

    // substr no case
    RUN_TEST( test_sv_substr_nocase_positive );
    RUN_TEST( test_sv_substr_nocase_negative );
    RUN_TEST( test_sv_substr_nocase_zero_length_needle );
    RUN_TEST( test_sv_substr_nocase_empty_view );
    RUN_TEST( test_sv_substr_nocase_unterminated );

    // non-terminated trim / size
    RUN_TEST( test_sv_left_trim_unterminated );
    RUN_TEST( test_sv_right_trim_unterminated );
    RUN_TEST( test_sv_trim_unterminated );
    RUN_TEST( test_sv_left_trim_respects_size );

    return UNITY_END();
}
