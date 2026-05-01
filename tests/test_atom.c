#include "unity.h"

#include <limits.h>
#include <string.h>

#include "../atom.h"

void setUp( void )
{
}

void tearDown( void )
{
}

// =============================================================================
// sand_atom_string  (from "C Interfaces and Implementations" by Hanson)
// =============================================================================

void test_atom_string_returns_non_null( void )
{
    const char* a = sand_atom_string( "hello" );

    TEST_ASSERT_NOT_NULL( a );
    TEST_ASSERT_EQUAL_STRING( "hello", a );
}

void test_atom_string_same_content_same_pointer( void )
{
    // Core interning property: identical strings yield the same pointer
    const char* a = sand_atom_string( "interned" );
    const char* b = sand_atom_string( "interned" );

    TEST_ASSERT_EQUAL_PTR( a, b );
}

void test_atom_string_different_content_different_pointer( void )
{
    const char* a = sand_atom_string( "alpha" );
    const char* b = sand_atom_string( "beta" );

    TEST_ASSERT_NOT_EQUAL( a, b );
}

void test_atom_string_preserves_content( void )
{
    const char* a = sand_atom_string( "sandlib" );

    TEST_ASSERT_EQUAL_STRING( "sandlib", a );
}

void test_atom_string_from_stack_buffer( void )
{
    // The atom should be independent of the original buffer
    char buf[ 32 ];
    strcpy( buf, "stack_test" );

    const char* a = sand_atom_string( buf );

    // Modify the original buffer
    buf[ 0 ] = 'X';

    // Atom should still hold the original value
    TEST_ASSERT_EQUAL_STRING( "stack_test", a );
}

void test_atom_string_empty( void )
{
    const char* a = sand_atom_string( "" );
    const char* b = sand_atom_string( "" );

    TEST_ASSERT_EQUAL_PTR( a, b );
    TEST_ASSERT_EQUAL_STRING( "", a );
}

// =============================================================================
// sand_atom_new
// =============================================================================

void test_atom_new_with_explicit_length( void )
{
    const char* a = sand_atom_new( "hello world", 5 );

    TEST_ASSERT_EQUAL_STRING( "hello", a );
}

void test_atom_new_zero_length( void )
{
    const char* a = sand_atom_new( "anything", 0 );

    TEST_ASSERT_NOT_NULL( a );
    TEST_ASSERT_EQUAL_STRING( "", a );
}

void test_atom_new_interning_with_length( void )
{
    const char* a = sand_atom_new( "hello world", 5 );   // "hello"
    const char* b = sand_atom_string( "hello" );

    TEST_ASSERT_EQUAL_PTR( a, b );
}

void test_atom_new_different_prefix_different_atom( void )
{
    const char* a = sand_atom_new( "hello world", 5 );   // "hello"
    const char* b = sand_atom_new( "hello world", 7 );   // "hello w"

    TEST_ASSERT_NOT_EQUAL( a, b );
}

// =============================================================================
// sand_atom_length
// =============================================================================

void test_atom_length_simple( void )
{
    const char* a = sand_atom_string( "test" );

    TEST_ASSERT_EQUAL_INT( 4, sand_atom_length( a ) );
}

void test_atom_length_empty( void )
{
    const char* a = sand_atom_string( "" );

    TEST_ASSERT_EQUAL_INT( 0, sand_atom_length( a ) );
}

void test_atom_length_long_string( void )
{
    const char* a = sand_atom_string( "a longer string for testing" );

    TEST_ASSERT_EQUAL_INT( 27, sand_atom_length( a ) );
}

// =============================================================================
// sand_atom_int
// =============================================================================

void test_atom_int_zero( void )
{
    const char* a = sand_atom_int( 0 );

    TEST_ASSERT_EQUAL_STRING( "0", a );
}

void test_atom_int_positive( void )
{
    const char* a = sand_atom_int( 42 );

    TEST_ASSERT_EQUAL_STRING( "42", a );
}

void test_atom_int_negative( void )
{
    const char* a = sand_atom_int( -7 );

    TEST_ASSERT_EQUAL_STRING( "-7", a );
}

void test_atom_int_large_number( void )
{
    const char* a = sand_atom_int( 1000000 );

    TEST_ASSERT_EQUAL_STRING( "1000000", a );
}

void test_atom_int_long_max( void )
{
    const char* a = sand_atom_int( LONG_MAX );

    // Convert LONG_MAX to string for comparison
    char expected[ 64 ];
    snprintf( expected, sizeof( expected ), "%ld", LONG_MAX );

    TEST_ASSERT_EQUAL_STRING( expected, a );
}

void test_atom_int_long_min( void )
{
    const char* a = sand_atom_int( LONG_MIN );

    char expected[ 64 ];
    snprintf( expected, sizeof( expected ), "%ld", LONG_MIN );

    TEST_ASSERT_EQUAL_STRING( expected, a );
}

void test_atom_int_interning( void )
{
    // Same integer should produce the same atom pointer
    const char* a = sand_atom_int( 123 );
    const char* b = sand_atom_int( 123 );

    TEST_ASSERT_EQUAL_PTR( a, b );
}

void test_atom_int_matches_string_atom( void )
{
    // atom_int(42) should return the same pointer as atom_string("42")
    const char* from_int    = sand_atom_int( 42 );
    const char* from_string = sand_atom_string( "42" );

    TEST_ASSERT_EQUAL_PTR( from_int, from_string );
}

// =============================================================================
// Stress / collision testing
// =============================================================================

void test_atom_many_unique_strings( void )
{
    // Insert many atoms and verify they all survive
    const char* atoms[ 100 ];
    char        buf[ 32 ];

    for ( int i = 0; i < 100; i++ )
    {
        snprintf( buf, sizeof( buf ), "atom_%d", i );
        atoms[ i ] = sand_atom_string( buf );
    }

    // Verify all are still correct
    for ( int i = 0; i < 100; i++ )
    {
        char expected[ 32 ];
        snprintf( expected, sizeof( expected ), "atom_%d", i );
        TEST_ASSERT_EQUAL_STRING( expected, atoms[ i ] );
    }
}

void test_atom_repeated_lookups( void )
{
    // Repeated lookups should always return the same pointer
    const char* first = sand_atom_string( "repeated" );

    for ( int i = 0; i < 50; i++ )
    {
        const char* again = sand_atom_string( "repeated" );
        TEST_ASSERT_EQUAL_PTR( first, again );
    }
}

// =============================================================================
// Main
// =============================================================================

int main( void )
{
    UNITY_BEGIN();

    // atom_string
    RUN_TEST( test_atom_string_returns_non_null );
    RUN_TEST( test_atom_string_same_content_same_pointer );
    RUN_TEST( test_atom_string_different_content_different_pointer );
    RUN_TEST( test_atom_string_preserves_content );
    RUN_TEST( test_atom_string_from_stack_buffer );
    RUN_TEST( test_atom_string_empty );

    // atom_new
    RUN_TEST( test_atom_new_with_explicit_length );
    RUN_TEST( test_atom_new_zero_length );
    RUN_TEST( test_atom_new_interning_with_length );
    RUN_TEST( test_atom_new_different_prefix_different_atom );

    // atom_length
    RUN_TEST( test_atom_length_simple );
    RUN_TEST( test_atom_length_empty );
    RUN_TEST( test_atom_length_long_string );

    // atom_int
    RUN_TEST( test_atom_int_zero );
    RUN_TEST( test_atom_int_positive );
    RUN_TEST( test_atom_int_negative );
    RUN_TEST( test_atom_int_large_number );
    RUN_TEST( test_atom_int_long_max );
    RUN_TEST( test_atom_int_long_min );
    RUN_TEST( test_atom_int_interning );
    RUN_TEST( test_atom_int_matches_string_atom );

    // stress
    RUN_TEST( test_atom_many_unique_strings );
    RUN_TEST( test_atom_repeated_lookups );

    return UNITY_END();
}
