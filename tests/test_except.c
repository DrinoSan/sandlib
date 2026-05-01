#include "unity.h"

#include "../except.h"

static sand_except_t Test_Error  = { "test error" };
static sand_except_t Other_Error = { "other error" };

void setUp( void )
{
    sand_except_stack = NULL;
}

void tearDown( void )
{
    sand_except_stack = NULL;
}

// =============================================================================
// Helpers for nested TRY (inner TRY in separate function to avoid
// the local variable 'sand_except_frame' shadowing the type name)
// =============================================================================

static void helper_raise_test_error_except_other( volatile int* inner_caught )
{
    TRY
        RAISE( Test_Error );
    EXCEPT( Other_Error )
        *inner_caught = 1;
    END_TRY;
}

static void helper_raise_then_finally( volatile int* finally_ran )
{
    TRY
        RAISE( Test_Error );
    FINALLY
        *finally_ran = 1;
    END_TRY;
}

static void helper_catch_and_reraise( volatile int* first_caught )
{
    TRY
        RAISE( Test_Error );
    EXCEPT( Test_Error )
        *first_caught = 1;
        RAISE( Other_Error );
    END_TRY;
}

static void helper_inner_catches( volatile int* caught )
{
    TRY
        RAISE( Test_Error );
    EXCEPT( Test_Error )
        *caught = 1;
    END_TRY;
}

// =============================================================================
// TRY / END_TRY basic flow
// =============================================================================

void test_try_body_executes( void )
{
    volatile int ran = 0;

    TRY
        ran = 1;
    END_TRY;

    TEST_ASSERT_EQUAL_INT( 1, ran );
}

void test_try_restores_stack_on_normal_flow( void )
{
    TEST_ASSERT_NULL( sand_except_stack );

    TRY
        TEST_ASSERT_NOT_NULL( sand_except_stack );
    END_TRY;

    TEST_ASSERT_NULL( sand_except_stack );
}

void test_stack_null_after_handled_exception( void )
{
    TRY
        RAISE( Test_Error );
    EXCEPT( Test_Error )
        // handled
    END_TRY;

    TEST_ASSERT_NULL( sand_except_stack );
}

// =============================================================================
// EXCEPT
// =============================================================================

void test_except_catches_matching_exception( void )
{
    volatile int caught = 0;

    TRY
        RAISE( Test_Error );
    EXCEPT( Test_Error )
        caught = 1;
    END_TRY;

    TEST_ASSERT_EQUAL_INT( 1, caught );
}

void test_except_handler_not_entered_on_normal_flow( void )
{
    volatile int handler_ran = 0;

    TRY
        // no exception
    EXCEPT( Test_Error )
        handler_ran = 1;
    END_TRY;

    TEST_ASSERT_EQUAL_INT( 0, handler_ran );
}

void test_except_nonmatching_propagates( void )
{
    volatile int inner_caught = 0;
    volatile int outer_caught = 0;

    TRY
        helper_raise_test_error_except_other( &inner_caught );
    EXCEPT( Test_Error )
        outer_caught = 1;
    END_TRY;

    TEST_ASSERT_EQUAL_INT( 0, inner_caught );
    TEST_ASSERT_EQUAL_INT( 1, outer_caught );
}

void test_except_multiple_handlers( void )
{
    volatile int which = 0;

    TRY
        RAISE( Other_Error );
    EXCEPT( Test_Error )
        which = 1;
    EXCEPT( Other_Error )
        which = 2;
    END_TRY;

    TEST_ASSERT_EQUAL_INT( 2, which );
}

void test_except_first_matching_handler_wins( void )
{
    volatile int which = 0;

    TRY
        RAISE( Test_Error );
    EXCEPT( Test_Error )
        which = 1;
    EXCEPT( Other_Error )
        which = 2;
    END_TRY;

    TEST_ASSERT_EQUAL_INT( 1, which );
}

void test_exception_identified_by_pointer( void )
{
    // Two different variables with the same reason string are different exceptions
    sand_except_t Another_Error = { "test error" };
    volatile int  caught_test   = 0;
    volatile int  caught_other  = 0;

    TRY
        RAISE( Another_Error );
    EXCEPT( Test_Error )
        caught_test = 1;
    ELSE
        caught_other = 1;
    END_TRY;

    TEST_ASSERT_EQUAL_INT( 0, caught_test );
    TEST_ASSERT_EQUAL_INT( 1, caught_other );
}

// =============================================================================
// ELSE (catch-all)
// =============================================================================

void test_else_catches_any_exception( void )
{
    volatile int caught = 0;

    TRY
        RAISE( Test_Error );
    ELSE
        caught = 1;
    END_TRY;

    TEST_ASSERT_EQUAL_INT( 1, caught );
}

void test_else_catches_unknown_exception( void )
{
    sand_except_t Unknown = { "unknown" };
    volatile int  caught  = 0;

    TRY
        RAISE( Unknown );
    ELSE
        caught = 1;
    END_TRY;

    TEST_ASSERT_EQUAL_INT( 1, caught );
}

void test_else_not_entered_on_normal_flow( void )
{
    volatile int handler_ran = 0;

    TRY
        // no exception
    ELSE
        handler_ran = 1;
    END_TRY;

    TEST_ASSERT_EQUAL_INT( 0, handler_ran );
}

// =============================================================================
// FINALLY
// =============================================================================

void test_finally_runs_on_normal_flow( void )
{
    volatile int finally_ran = 0;

    TRY
        // no exception
    FINALLY
        finally_ran = 1;
    END_TRY;

    TEST_ASSERT_EQUAL_INT( 1, finally_ran );
}

void test_finally_runs_on_exception( void )
{
    volatile int finally_ran = 0;
    volatile int caught      = 0;

    TRY
        helper_raise_then_finally( &finally_ran );
    EXCEPT( Test_Error )
        caught = 1;
    END_TRY;

    TEST_ASSERT_EQUAL_INT( 1, finally_ran );
    TEST_ASSERT_EQUAL_INT( 1, caught );
}

void test_except_then_finally( void )
{
    volatile int caught      = 0;
    volatile int finally_ran = 0;

    TRY
        RAISE( Test_Error );
    EXCEPT( Test_Error )
        caught = 1;
    FINALLY
        finally_ran = 1;
    END_TRY;

    TEST_ASSERT_EQUAL_INT( 1, caught );
    TEST_ASSERT_EQUAL_INT( 1, finally_ran );
}

// =============================================================================
// Nested TRY (inner TRY in helper functions)
// =============================================================================

void test_nested_try_inner_catches( void )
{
    volatile int inner_caught = 0;
    volatile int continued    = 0;

    TRY
        helper_inner_catches( &inner_caught );
        continued = 1;
    END_TRY;

    TEST_ASSERT_EQUAL_INT( 1, inner_caught );
    TEST_ASSERT_EQUAL_INT( 1, continued );
}

void test_raise_from_except_handler( void )
{
    volatile int first_caught  = 0;
    volatile int second_caught = 0;

    TRY
        helper_catch_and_reraise( &first_caught );
    EXCEPT( Other_Error )
        second_caught = 1;
    END_TRY;

    TEST_ASSERT_EQUAL_INT( 1, first_caught );
    TEST_ASSERT_EQUAL_INT( 1, second_caught );
}

// =============================================================================
// Main
// =============================================================================

int main( void )
{
    UNITY_BEGIN();

    // TRY basic flow
    RUN_TEST( test_try_body_executes );
    RUN_TEST( test_try_restores_stack_on_normal_flow );
    RUN_TEST( test_stack_null_after_handled_exception );

    // EXCEPT
    RUN_TEST( test_except_catches_matching_exception );
    RUN_TEST( test_except_handler_not_entered_on_normal_flow );
    RUN_TEST( test_except_nonmatching_propagates );
    RUN_TEST( test_except_multiple_handlers );
    RUN_TEST( test_except_first_matching_handler_wins );
    RUN_TEST( test_exception_identified_by_pointer );

    // ELSE
    RUN_TEST( test_else_catches_any_exception );
    RUN_TEST( test_else_catches_unknown_exception );
    RUN_TEST( test_else_not_entered_on_normal_flow );

    // FINALLY
    RUN_TEST( test_finally_runs_on_normal_flow );
    RUN_TEST( test_finally_runs_on_exception );
    RUN_TEST( test_except_then_finally );

    // Nested
    RUN_TEST( test_nested_try_inner_catches );
    RUN_TEST( test_raise_from_except_handler );

    return UNITY_END();
}
