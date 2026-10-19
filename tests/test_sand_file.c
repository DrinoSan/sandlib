#include "unity.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../sand_file.h"

#define TEMP_PATH    "/tmp/sandlib_test_sand_file.bin"
#define MISSING_PATH "/tmp/sandlib_test_sand_file_missing.bin"

static sand_file_t file;

static void write_temp( const void* bytes, size_t n )
{
    FILE* out = fopen( TEMP_PATH, "wb" );

    TEST_ASSERT_NOT_NULL( out );

    if ( n > 0 )
    {
        size_t wrote = fwrite( bytes, 1, n, out );
        TEST_ASSERT_EQUAL_size_t( n, wrote );
    }

    TEST_ASSERT_EQUAL_INT( 0, fclose( out ) );
}

void setUp( void )
{
    remove( TEMP_PATH );
    remove( MISSING_PATH );
    sand_file_create( &file );
}

void tearDown( void )
{
    sand_string_destroy( &file.content );
    remove( TEMP_PATH );
    remove( MISSING_PATH );
}

// =============================================================================
// sand_file_create
// =============================================================================

void test_create_initializes_handler( void )
{
    TEST_ASSERT_NULL( file.fp );
    TEST_ASSERT_NULL( file.content.data );
    TEST_ASSERT_EQUAL_INT32( 0, file.content.size );
    TEST_ASSERT_EQUAL_INT32( 0, file.content.capacity );
}

// =============================================================================
// sand_file_open_and_read
// =============================================================================

void test_read_text_file( void )
{
    const char text[] = "hello sand\n";

    write_temp( text, strlen( text ) );
    sand_file_open_and_read( &file, TEMP_PATH );

    TEST_ASSERT_EQUAL_INT32( (int32_t)strlen( text ), file.content.size );
    TEST_ASSERT_EQUAL_INT(
        0, memcmp( file.content.data, text, strlen( text ) ) );
}

void test_read_empty_file( void )
{
    write_temp( "", 0 );
    sand_file_open_and_read( &file, TEMP_PATH );

    TEST_ASSERT_EQUAL_INT32( 0, file.content.size );
}

void test_read_missing_file( void )
{
    sand_file_open_and_read( &file, MISSING_PATH );

    TEST_ASSERT_NULL( file.fp );
    TEST_ASSERT_NULL( file.content.data );
    TEST_ASSERT_EQUAL_INT32( 0, file.content.size );
    TEST_ASSERT_EQUAL_INT32( 0, file.content.capacity );
}

void test_read_binary_file( void )
{
    const unsigned char bytes[] = { 0x00, 0x01, 0xff, 0x00, 0x7f, '\n' };

    write_temp( bytes, sizeof( bytes ) );
    sand_file_open_and_read( &file, TEMP_PATH );

    TEST_ASSERT_EQUAL_INT32( (int32_t)sizeof( bytes ), file.content.size );
    TEST_ASSERT_EQUAL_INT(
        0, memcmp( file.content.data, bytes, sizeof( bytes ) ) );
}

void test_read_is_nul_terminated( void )
{
    TEST_IGNORE_MESSAGE(
        "Bekannter Bug: sand_file.c:27 terminiert nach fread nicht "
        "siehe DrinoSan/Fun docs/sandlib-TODO.md" );

    const char text[] = "hello";
    void*      poison[ 32 ];

    write_temp( text, strlen( text ) );

    /* sand_grow_string's first allocation is 10 bytes and is not cleared.
       Fill that bin so a missing terminator is not hidden by a zero page. */
    for ( int i = 0; i < 32; i++ )
    {
        poison[ i ] = malloc( 10 );
        TEST_ASSERT_NOT_NULL( poison[ i ] );
        memset( poison[ i ], 'A', 10 );
    }
    for ( int i = 0; i < 32; i++ )
    {
        free( poison[ i ] );
    }

    sand_file_open_and_read( &file, TEMP_PATH );

    TEST_ASSERT_EQUAL_INT32( (int32_t)strlen( text ), file.content.size );
    TEST_ASSERT_NOT_NULL( file.content.data );
    TEST_ASSERT_EQUAL_CHAR( '\0', file.content.data[ file.content.size ] );
}

// =============================================================================
// Main
// =============================================================================

int main( void )
{
    UNITY_BEGIN();

    RUN_TEST( test_create_initializes_handler );
    RUN_TEST( test_read_text_file );
    RUN_TEST( test_read_empty_file );
    RUN_TEST( test_read_missing_file );
    RUN_TEST( test_read_binary_file );
    RUN_TEST( test_read_is_nul_terminated );

    return UNITY_END();
}
