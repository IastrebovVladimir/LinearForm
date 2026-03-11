#include "../linearform.h"
#include <stdio.h>
#include <string.h>
#include "asserts.h"

int tests_run = 0;
int tests_failed = 0;

int test_create_and_value_int(void) {
    const TypeInfo *type = get_int_t();
    LinearForm *f = createForm(type, 3);
    ASSERT_TRUE(f != NULL);
    ASSERT_INT_EQ(3, (int)f->n);

    int c0 = 1, c1 = 2, c2 = 3;
    setcoeff(f, 0, &c0);
    setcoeff(f, 1, &c1);
    setcoeff(f, 2, &c2);

    int args[3] = {10, 1, -2};
    int result = 0;
    evaluateForm(f, args, 3, &result);

    ASSERT_INT_EQ(6, result);

    deleteForm(f);
    return 1;
}

int test_scalar_int(void) {
    const TypeInfo *type = get_int_t();
    LinearForm *f = createForm(type, 2);
    ASSERT_TRUE(f != NULL);

    int c0 = 2, c1 = -3;
    setcoeff(f, 0, &c0);
    setcoeff(f, 1, &c1);

    int k = 5;
    scalar(f, &k);

    int args[2] = {1, 1};
    int result = 0;
    evaluateForm(f, args, 2, &result);

    ASSERT_INT_EQ(-5, result);

    deleteForm(f);
    return 1;
}

int test_addition_int(void) {
    const TypeInfo *type = get_int_t();
    LinearForm *f1 = createForm(type, 3);
    ASSERT_TRUE(f1 != NULL);
    int a0 = 1, a1 = 2, a2 = 3;
    setcoeff(f1, 0, &a0);
    setcoeff(f1, 1, &a1);
    setcoeff(f1, 2, &a2);
    LinearForm *f2 = createForm(type, 3);
    ASSERT_TRUE(f2 != NULL);
    int b0 = 4, b1 = -1, b2 = 0;
    setcoeff(f2, 0, &b0);
    setcoeff(f2, 1, &b1);
    setcoeff(f2, 2, &b2);

    LinearForm *sum = addition(f1, f2);
    ASSERT_TRUE(sum != NULL);
    ASSERT_INT_EQ(3, (int)sum->n);

    int args[3] = {1, 1, 1};
    int res = 0;
    evaluateForm(sum, args, 3, &res);

    ASSERT_INT_EQ(9, res);

    deleteForm(f1);
    deleteForm(f2);
    deleteForm(sum);
    return 1;
}
int test_subtraction_int(void) {
    const TypeInfo *type = get_int_t();

    LinearForm *f1 = createForm(type, 2);
    ASSERT_TRUE(f1 != NULL);
    int a0 = 5, a1 = 0;
    setcoeff(f1, 0, &a0);
    setcoeff(f1, 1, &a1);

    LinearForm *f2 = createForm(type, 2);
    ASSERT_TRUE(f2 != NULL);
    int b0 = 2, b1 = 7;
    setcoeff(f2, 0, &b0);
    setcoeff(f2, 1, &b1);

    LinearForm *diff = subtraction(f1, f2);
    ASSERT_TRUE(diff != NULL);
    ASSERT_INT_EQ(2, (int)diff->n);

    int args[2] = {1, 1};
    int res = 0;
    evaluateForm(diff, args, 2, &res);

    ASSERT_INT_EQ(-4, res);

    deleteForm(f1);
    deleteForm(f2);
    deleteForm(diff);
    return 1;
}
int test_scalar_and_value_int(void) {
    const TypeInfo *t = get_int_t();

    LinearForm *f = createForm(t, 2);
    ASSERT_TRUE(f != NULL);
    int c0 = 1, c1 = -2;
    setcoeff(f, 0, &c0);
    setcoeff(f, 1, &c1);

    int k = 3;
    scalar(f, &k);

    int args[2] = {2, 1};
    int res = 0;
    evaluateForm(f, args, 2, &res);

    ASSERT_INT_EQ(0, res);

    deleteForm(f);
    return 1;
}

int test_addition_f1_f2_int(void) {
    const TypeInfo *type = get_int_t();

    LinearForm *f1 = createForm(type, 2);
    LinearForm *f2 = createForm(type, 2);
    ASSERT_TRUE(f1 && f2);

    int a0 = 1, a1 = 2;
    setcoeff(f1, 0, &a0);
    setcoeff(f1, 1, &a1);

    int b0 = 3, b1 = -1;
    setcoeff(f2, 0, &b0);
    setcoeff(f2, 1, &b1);

    LinearForm *s1 = addition(f1, f2);
    ASSERT_TRUE(s1);
    ASSERT_TRUE(s1->operations[0] && s1->operations[1]);
    ASSERT_INT_EQ(0, strcmp(s1->operations[0], "1+3"));
    ASSERT_INT_EQ(0, strcmp(s1->operations[1], "2-1"));

    deleteForm(f1);
    deleteForm(f2);
    deleteForm(s1);
    return 1;
}

int test_addition_f3_f4_int(void) {
    const TypeInfo *type = get_int_t();

    LinearForm *f3 = createForm(type, 2);
    LinearForm *f4 = createForm(type, 2);
    ASSERT_TRUE(f3 && f4);

    int c0 = 0, c1 = 4;
    setcoeff(f3, 0, &c0);
    setcoeff(f3, 1, &c1);

    int d0 = -2, d1 = 5;
    setcoeff(f4, 0, &d0);
    setcoeff(f4, 1, &d1);

    LinearForm *s2 = addition(f3, f4);
    ASSERT_TRUE(s2);
    ASSERT_TRUE(s2->operations[0] && s2->operations[1]);
    ASSERT_INT_EQ(0, strcmp(s2->operations[0], "0-2"));
    ASSERT_INT_EQ(0, strcmp(s2->operations[1], "4+5"));

    deleteForm(f3);
    deleteForm(f4);
    deleteForm(s2);
    return 1;
}

int test_subtraction_op_int(void) {
    const TypeInfo *type = get_int_t();

    LinearForm *f1 = createForm(type, 2);
    LinearForm *f2 = createForm(type, 2);
    ASSERT_TRUE(f1 && f2);
    int a0 = 1, a1 = 2;
    int b0 = 3, b1 = -1;
    setcoeff(f1, 0, &a0);
    setcoeff(f1, 1, &a1);
    setcoeff(f2, 0, &b0);
    setcoeff(f2, 1, &b1);
    LinearForm *s1 = addition(f1, f2);
    ASSERT_TRUE(s1);

    LinearForm *f3 = createForm(type, 2);
    LinearForm *f4 = createForm(type, 2);
    ASSERT_TRUE(f3 && f4);
    int c0 = 0, c1 = 4;
    int d0 = -2, d1 = 5;
    setcoeff(f3, 0, &c0);
    setcoeff(f3, 1, &c1);
    setcoeff(f4, 0, &d0);
    setcoeff(f4, 1, &d1);
    LinearForm *s2 = addition(f3, f4);
    ASSERT_TRUE(s2);

    LinearForm *h = subtraction(s1, s2);
    ASSERT_TRUE(h);
    ASSERT_TRUE(h->operations[0] && h->operations[1]);
    ASSERT_INT_EQ(0, strcmp(h->operations[0], "1+3-0+2"));
    ASSERT_INT_EQ(0, strcmp(h->operations[1], "2-1-4-5"));

    deleteForm(f1);
    deleteForm(f2);
    deleteForm(f3);
    deleteForm(f4);
    deleteForm(s1);
    deleteForm(s2);
    deleteForm(h);
    return 1;
}

int test_scalar_op_int(void) {
    const TypeInfo *type = get_int_t();

    LinearForm *f1 = createForm(type, 2);
    LinearForm *f2 = createForm(type, 2);
    LinearForm *f3 = createForm(type, 2);
    LinearForm *f4 = createForm(type, 2);
    ASSERT_TRUE(f1 && f2 && f3 && f4);

    int a0 = 1, a1 = 2;
    int b0 = 3, b1 = -1;
    int c0 = 0, c1 = 4;
    int d0 = -2, d1 = 5;

    setcoeff(f1, 0, &a0);
    setcoeff(f1, 1, &a1);
    setcoeff(f2, 0, &b0);
    setcoeff(f2, 1, &b1);
    setcoeff(f3, 0, &c0);
    setcoeff(f3, 1, &c1);
    setcoeff(f4, 0, &d0);
    setcoeff(f4, 1, &d1);

    LinearForm *s1 = addition(f1, f2);
    LinearForm *s2 = addition(f3, f4);
    LinearForm *h  = subtraction(s1, s2);
    ASSERT_TRUE(s1 && s2 && h);

    int k = 2;
    scalar(h, &k);
    ASSERT_TRUE(h->operations[0] && h->operations[1]);
    ASSERT_INT_EQ(0, strcmp(h->operations[0], "2*1+2*3-2*0+2*2"));
    ASSERT_INT_EQ(0, strcmp(h->operations[1], "2*2-2*1-2*4-2*5"));

    int args[2] = {1, 1};
    int result = 0;
    evaluateForm(h, args, 2, &result);
    ASSERT_INT_EQ(-4, result);

    deleteForm(f1);
    deleteForm(f2);
    deleteForm(f3);
    deleteForm(f4);
    deleteForm(s1);
    deleteForm(s2);
    deleteForm(h);
    return 1;
}

int test_types_equal_1(void) {
    const TypeInfo *t1 = get_int_t();
    const TypeInfo *t2 = get_int_t();

    ASSERT_TRUE(t1 == t2);
    return 1;
}

int test_types_equal_2(void) {
    const TypeInfo *ti = get_int_t();
    const TypeInfo *td = get_double_t();

    ASSERT_TRUE(ti != td);
    return 1;
}

int test_addition_types(void) {
    LinearForm *f1 = createForm(get_int_t(), 2);
    LinearForm *f2 = createForm(get_double_t(), 2);
    ASSERT_TRUE(f1 && f2);

    int a0 = 1, a1 = 2;
    double b0 = 1.5, b1 = -3.0;

    setcoeff(f1, 0, &a0);
    setcoeff(f1, 1, &a1);

    setcoeff(f2, 0, &b0);
    setcoeff(f2, 1, &b1);

    LinearForm *sum = addition(f1, f2);
    ASSERT_TRUE(sum == NULL);

    deleteForm(f1);
    deleteForm(f2);
    return 1;
}



