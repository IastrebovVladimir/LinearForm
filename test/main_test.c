#include <stdio.h>
#include "asserts.h"

int test_create_and_value_int(void);
int test_scalar_int(void);
int test_addition_int(void);
int test_subtraction_int(void);
int test_scalar_and_value_int(void);
int test_addition_f1_f2_int(void);
int test_addition_f3_f4_int(void);
int test_subtraction_op_int(void);
int test_scalar_op_int(void);
int test_types_equal_1(void);
int test_types_equal_2(void);
int test_addition_types(void);


int main(void) {
    RUN_TEST(test_create_and_value_int);
    RUN_TEST(test_scalar_int);
    RUN_TEST(test_addition_int);
    RUN_TEST(test_subtraction_int);
    RUN_TEST(test_scalar_and_value_int);
    RUN_TEST(test_addition_f1_f2_int);
    RUN_TEST(test_addition_f3_f4_int);
    RUN_TEST(test_subtraction_op_int);
    RUN_TEST(test_scalar_op_int);
    RUN_TEST(test_types_equal_1);
    RUN_TEST(test_types_equal_2);
    RUN_TEST(test_addition_types);

    printf("\nTests run: %d, failed: %d\n", tests_run, tests_failed);
    return tests_failed ? 1 : 0;
}

