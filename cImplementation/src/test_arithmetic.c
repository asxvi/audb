#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "arithmetic.h"

#define CU_ASSERT_EQUAL_MSG(actual, expected) \
    do { \
        if ((actual) != (expected)) { \
            printf("Assertion failed: %s != %s\n  Expected: %d, Got: %d\n", \
                   #actual, #expected, (expected), (actual)); \
            CU_FAIL("Values not equal"); \
        } \
    } while (0)


void testRangeAdd(){
    Int4Range a = {1, 3};
    Int4Range b = {9, 10};
    Int4Range r1 = range_add(a, b);
    CU_ASSERT(r1.lower == 10);
    CU_ASSERT(r1.upper == 12);
    Int4Range r2 = range_add(b, a);
    CU_ASSERT(r2.lower == 10);
    CU_ASSERT(r2.upper == 12);

    // invalid range (lower >= upper)
    Int4Range badA = {300, 1};
    Int4Range badB = {10, 10};
    Int4Range r3 = range_add(badA, b);
    CU_ASSERT(r3.lower == 0);
    CU_ASSERT(r3.upper == 0);
    Int4Range r4 = range_add(a, badB);
    CU_ASSERT(r4.lower == 0);
    CU_ASSERT(r4.upper == 0); 
}

void testRangeSubtract(){
    Int4Range a = {1, 3};
    Int4Range b = {9, 10};
    Int4Range r1 = range_subtract(a, b);
    CU_ASSERT(r1.lower == -8);
    CU_ASSERT(r1.upper == -6);
    Int4Range r2 = range_subtract(b, a);
    CU_ASSERT(r2.lower == 7);
    CU_ASSERT(r2.upper == 9);
    
    // inlcude macro with message on fail
    // CU_ASSERT_EQUAL_MSG(r1.lower, -8);

    // invalid range (lower >= upper)
    Int4Range badA = {300, 1};
    Int4Range badB = {10, 10};
    Int4Range r3 = range_subtract(badA, b);
    CU_ASSERT(r3.lower == 0);
    CU_ASSERT(r3.upper == 0);
    Int4Range r4 = range_subtract(a, badB);
    CU_ASSERT(r4.lower == 0);
    CU_ASSERT(r4.upper == 0); 
}

int main(){
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("AddTestSuit", 0, 0);
    CU_add_test(suite, "testRangeAdd: ", testRangeAdd);
    CU_add_test(suite, "testRangeSubtract: ", testRangeSubtract);
    CU_basic_run_tests();
    CU_cleanup_registry();
    

    return 0;
}