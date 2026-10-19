#include "unity.h"

#include <stdlib.h>
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

// API does not allow empty strings
//void test_append_empty_string( void )
//{
//    sand_string_append( &s, "" );
//
//    TEST_ASSERT_EQUAL_INT32( 0, s.size );
//}

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
// HasPrefix hasSuffix tests
// =============================================================================
void test_prefix_positive( void )
{
    sand_string_append( &s, "hello" );

    int has_prefix = sand_string_has_prefix( &s, "hel" );
    TEST_ASSERT_TRUE( has_prefix );
}

void test_prefix_negative( void )
{
    sand_string_append( &s, "hello" );

    int has_prefix = sand_string_has_prefix( &s, "hela" );
    TEST_ASSERT_FALSE( has_prefix );
}

void test_prefix_too_long( void )
{
    sand_string_append( &s, "hello" );

    int has_prefix = sand_string_has_prefix( &s, "hello world" );
    TEST_ASSERT_FALSE( has_prefix );
}

void test_suffix_positive( void )
{
    sand_string_append( &s, "hello" );

    int has_suffix = sand_string_has_suffix( &s, "llo" );
    TEST_ASSERT_TRUE( has_suffix );
}

void test_suffix_negative( void )
{
    sand_string_append( &s, "hello" );

    int has_suffix = sand_string_has_suffix( &s, "ella" );
    TEST_ASSERT_FALSE( has_suffix );
}

void test_suffix_too_long( void )
{
    sand_string_append( &s, "hello" );

    int has_suffix = sand_string_has_prefix( &s, "hello world" );
    TEST_ASSERT_FALSE( has_suffix );
}

void test_substr_positive( void )
{
    sand_string_append( &s, "sandrino" );
    bool has_substr = sand_string_has_substr( &s, "and" );

    TEST_ASSERT_TRUE( has_substr );
}

void test_substr_negative( void )
{
    sand_string_append( &s, "sandrino" );
    bool has_substr = sand_string_has_substr( &s, "andd" );

    TEST_ASSERT_FALSE( has_substr );
}

void test_substr_too_long( void )
{
    sand_string_append( &s, "sandrino" );
    bool has_substr = sand_string_has_substr( &s, "sandrino hello world" );

    TEST_ASSERT_FALSE( has_substr );
}

// =============================================================================
// sand_grow_string
// =============================================================================

void test_grow_from_empty( void )
{
    sand_grow_string( &s, 4 );

    TEST_ASSERT_NOT_NULL( s.data );
    TEST_ASSERT_EQUAL_INT32( 0, s.size );
    TEST_ASSERT_TRUE( s.capacity >= 4 );
}

void test_grow_zero_length( void )
{
    sand_grow_string( &s, 0 );

    TEST_ASSERT_NOT_NULL( s.data );
    TEST_ASSERT_EQUAL_INT32( 0, s.size );
    TEST_ASSERT_TRUE( s.capacity > 0 );
}

void test_grow_preserves_existing_bytes( void )
{
    sand_string_append( &s, "hello" );
    int32_t cap = s.capacity;

    sand_grow_string( &s, 100 );

    TEST_ASSERT_TRUE( s.capacity > cap );
    TEST_ASSERT_EQUAL_INT32( 5, s.size );
    TEST_ASSERT_EQUAL_INT( 0, memcmp( s.data, "hello", 5 ) );
}

// =============================================================================
// sand_string_clear
// =============================================================================

void test_clear_resets_to_zero( void )
{
    sand_string_append( &s, "hello" );
    // clear drops the buffer without freeing it.
    char* orphan = s.data;

    sand_string_clear( &s );

    TEST_ASSERT_NULL( s.data );
    TEST_ASSERT_EQUAL_INT32( 0, s.size );
    TEST_ASSERT_EQUAL_INT32( 0, s.capacity );

    free( orphan );
}

// =============================================================================
// trim on an empty Sand_string
// =============================================================================

void test_left_trim_empty_string( void )
{
    TEST_IGNORE_MESSAGE(
        "Bekannter Bug: sand_string_left_trim auf leerem String "
        "dereferenziert NULL siehe DrinoSan/Fun docs/sandlib-TODO.md" );

    sand_string_left_trim( &s, 0 );

    TEST_ASSERT_NULL( s.data );
    TEST_ASSERT_EQUAL_INT32( 0, s.size );
}

void test_right_trim_empty_string( void )
{
    TEST_IGNORE_MESSAGE(
        "Bekannter Bug: trim auf leerem String ruft strlen(NULL) auf "
        "siehe DrinoSan/Fun docs/sandlib-TODO.md" );

    sand_string_right_trim( &s, 0 );

    TEST_ASSERT_NULL( s.data );
    TEST_ASSERT_EQUAL_INT32( 0, s.size );
}

void test_trim_empty_string( void )
{
    TEST_IGNORE_MESSAGE(
        "Bekannter Bug: sand_string_trim auf leerem String ruft "
        "sand_string_left_trim auf, das NULL dereferenziert "
        "siehe DrinoSan/Fun docs/sandlib-TODO.md" );

    sand_string_trim( &s, 0 );

    TEST_ASSERT_NULL( s.data );
    TEST_ASSERT_EQUAL_INT32( 0, s.size );
}

void test_trim_cstr_empty( void )
{
    char buf[] = "";

    sand_string_trim_cstr( buf, 0 );

    TEST_ASSERT_EQUAL_STRING( "", buf );
}

// =============================================================================
// sand_string_has_prefix_n / empty prefix
// =============================================================================

void test_prefix_empty_string( void )
{
    TEST_ASSERT_FALSE( sand_string_has_prefix( &s, "a" ) );
    TEST_ASSERT_TRUE( sand_string_has_prefix( &s, "" ) );
}

void test_prefix_n_positive( void )
{
    sand_string_append( &s, "hello" );

    TEST_ASSERT_TRUE( sand_string_has_prefix_n( &s, "hel", 3 ) );
}

void test_prefix_n_negative( void )
{
    sand_string_append( &s, "hello" );

    TEST_ASSERT_FALSE( sand_string_has_prefix_n( &s, "hex", 3 ) );
}

void test_prefix_n_too_long( void )
{
    sand_string_append( &s, "hello" );

    TEST_ASSERT_FALSE( sand_string_has_prefix_n( &s, "hello!", 6 ) );
}

void test_prefix_n_zero_length( void )
{
    char raw[ 1 ] = { 'Z' };

    sand_string_append( &s, "hello" );

    TEST_ASSERT_TRUE( sand_string_has_prefix_n( &s, raw, 0 ) );
}

void test_prefix_n_empty_string( void )
{
    TEST_ASSERT_FALSE( sand_string_has_prefix_n( &s, "h", 1 ) );
    TEST_ASSERT_TRUE( sand_string_has_prefix_n( &s, "h", 0 ) );
}

void test_prefix_n_unterminated( void )
{
    char raw[ 4 ] = { 'h', 'e', 'X', 'Y' };

    sand_string_append( &s, "hello" );

    TEST_ASSERT_TRUE( sand_string_has_prefix_n( &s, raw, 2 ) );
    TEST_ASSERT_FALSE( sand_string_has_prefix_n( &s, raw, 3 ) );
}

// =============================================================================
// sand_string_has_suffix_n / empty suffix
// =============================================================================

void test_suffix_empty_string( void )
{
    TEST_ASSERT_FALSE( sand_string_has_suffix( &s, "a" ) );
    TEST_ASSERT_TRUE( sand_string_has_suffix( &s, "" ) );
}

void test_suffix_n_positive( void )
{
    sand_string_append( &s, "hello" );

    TEST_ASSERT_TRUE( sand_string_has_suffix_n( &s, "llo", 3 ) );
}

void test_suffix_n_negative( void )
{
    sand_string_append( &s, "hello" );

    TEST_ASSERT_FALSE( sand_string_has_suffix_n( &s, "lla", 3 ) );
}

void test_suffix_n_too_long( void )
{
    sand_string_append( &s, "hello" );

    TEST_ASSERT_FALSE( sand_string_has_suffix_n( &s, "hello!", 6 ) );
}

void test_suffix_n_zero_length( void )
{
    char raw[ 1 ] = { 'Z' };

    sand_string_append( &s, "hello" );

    TEST_ASSERT_TRUE( sand_string_has_suffix_n( &s, raw, 0 ) );
}

void test_suffix_n_empty_string( void )
{
    TEST_ASSERT_FALSE( sand_string_has_suffix_n( &s, "o", 1 ) );
    TEST_ASSERT_TRUE( sand_string_has_suffix_n( &s, "o", 0 ) );
}

void test_suffix_n_unterminated( void )
{
    char raw[ 4 ] = { 'l', 'l', 'o', 'X' };

    sand_string_append( &s, "hello" );

    TEST_ASSERT_TRUE( sand_string_has_suffix_n( &s, raw, 3 ) );
    TEST_ASSERT_FALSE( sand_string_has_suffix_n( &s, raw, 4 ) );
}

void test_suffix_n_honors_size( void )
{
    sand_string_append( &s, "hello!" );
    s.size = 5;

    TEST_ASSERT_TRUE( sand_string_has_suffix_n( &s, "llo", 3 ) );
    TEST_ASSERT_FALSE( sand_string_has_suffix_n( &s, "o!", 2 ) );
}

// =============================================================================
// sand_string_has_substr_n / empty substr
// =============================================================================

void test_substr_empty_string( void )
{
    TEST_ASSERT_FALSE( sand_string_has_substr( &s, "a" ) );
    TEST_ASSERT_TRUE( sand_string_has_substr( &s, "" ) );
}

void test_substr_zero_length_needle( void )
{
    sand_string_append( &s, "sand" );

    TEST_ASSERT_TRUE( sand_string_has_substr( &s, "" ) );
}

void test_substr_n_positive( void )
{
    sand_string_append( &s, "sandrino" );

    TEST_ASSERT_TRUE( sand_string_has_substr_n( &s, "and", 3 ) );
}

void test_substr_n_negative( void )
{
    sand_string_append( &s, "sandrino" );

    TEST_ASSERT_FALSE( sand_string_has_substr_n( &s, "andd", 4 ) );
}

void test_substr_n_too_long( void )
{
    sand_string_append( &s, "sandrino" );

    TEST_ASSERT_FALSE(
        sand_string_has_substr_n( &s, "sandrino hello", 14 ) );
}

void test_substr_n_zero_length( void )
{
    char raw[ 1 ] = { 'Z' };

    sand_string_append( &s, "sandrino" );

    TEST_ASSERT_TRUE( sand_string_has_substr_n( &s, raw, 0 ) );
}

void test_substr_n_empty_string( void )
{
    TEST_ASSERT_FALSE( sand_string_has_substr_n( &s, "a", 1 ) );
    TEST_ASSERT_TRUE( sand_string_has_substr_n( &s, "a", 0 ) );
}

void test_substr_n_unterminated( void )
{
    char raw[ 4 ] = { 'a', 'n', 'd', 'X' };

    sand_string_append( &s, "sandrino" );

    TEST_ASSERT_TRUE( sand_string_has_substr_n( &s, raw, 3 ) );
    TEST_ASSERT_FALSE( sand_string_has_substr_n( &s, raw, 4 ) );
}

void test_substr_n_honors_size( void )
{
    sand_string_append( &s, "sandX" );
    s.size = 4;

    TEST_ASSERT_TRUE( sand_string_has_substr_n( &s, "sand", 4 ) );
    TEST_ASSERT_FALSE( sand_string_has_substr_n( &s, "X", 1 ) );
    TEST_ASSERT_FALSE( sand_string_has_substr_n( &s, "dX", 2 ) );
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
    // API DOES NOT ALLOW EMPYT STRINGS
    //RUN_TEST( test_append_empty_string );
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

    // Prefix tests
    RUN_TEST( test_prefix_positive );
    RUN_TEST( test_prefix_negative );
    RUN_TEST( test_prefix_too_long );
    // Suffix tests
    RUN_TEST( test_suffix_positive );
    RUN_TEST( test_suffix_negative );
    RUN_TEST( test_suffix_too_long );

    // SubStr tests
    RUN_TEST( test_substr_positive );
    RUN_TEST( test_substr_negative );
    RUN_TEST( test_substr_too_long );

    // grow
    RUN_TEST( test_grow_from_empty );
    RUN_TEST( test_grow_zero_length );
    RUN_TEST( test_grow_preserves_existing_bytes );

    // clear
    RUN_TEST( test_clear_resets_to_zero );

    // empty trim
    RUN_TEST( test_left_trim_empty_string );
    RUN_TEST( test_right_trim_empty_string );
    RUN_TEST( test_trim_empty_string );
    RUN_TEST( test_trim_cstr_empty );

    // prefix_n
    RUN_TEST( test_prefix_empty_string );
    RUN_TEST( test_prefix_n_positive );
    RUN_TEST( test_prefix_n_negative );
    RUN_TEST( test_prefix_n_too_long );
    RUN_TEST( test_prefix_n_zero_length );
    RUN_TEST( test_prefix_n_empty_string );
    RUN_TEST( test_prefix_n_unterminated );

    // suffix_n
    RUN_TEST( test_suffix_empty_string );
    RUN_TEST( test_suffix_n_positive );
    RUN_TEST( test_suffix_n_negative );
    RUN_TEST( test_suffix_n_too_long );
    RUN_TEST( test_suffix_n_zero_length );
    RUN_TEST( test_suffix_n_empty_string );
    RUN_TEST( test_suffix_n_unterminated );
    RUN_TEST( test_suffix_n_honors_size );

    // substr_n
    RUN_TEST( test_substr_empty_string );
    RUN_TEST( test_substr_zero_length_needle );
    RUN_TEST( test_substr_n_positive );
    RUN_TEST( test_substr_n_negative );
    RUN_TEST( test_substr_n_too_long );
    RUN_TEST( test_substr_n_zero_length );
    RUN_TEST( test_substr_n_empty_string );
    RUN_TEST( test_substr_n_unterminated );
    RUN_TEST( test_substr_n_honors_size );

    return UNITY_END();
}
