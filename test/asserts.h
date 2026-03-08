#ifndef ASSERTS_H
#define ASSERTS_H
#include <stdio.h>

#define ASSERT_TRUE(expr) do {                                   \
    if (!(expr)) {                                               \
        printf("  ASSERT_TRUE failed: %s (line %d)\n",           \
               #expr, __LINE__);                                 \
        return 0;                                                \
    }                                                            \
} while (0)

#define ASSERT_INT_EQ(exp, act) do {                             \
    int _e = (exp);                                              \
    int _a = (act);                                              \
    if (_e != _a) {                                              \
        printf("  ASSERT_INT_EQ failed: expected %d, got %d "    \
               "(%s vs %s, line %d)\n",                          \
               _e, _a, #exp, #act, __LINE__);                    \
        return 0;                                                \
    }                                                            \
} while (0)

extern int tests_run;
extern int tests_failed;

#define RUN_TEST(test_fn) do {                                   \
    tests_run++;                                                 \
    printf("Running %s... ", #test_fn);                          \
    if (test_fn()) {                                             \
        printf("OK\n");                                          \
    } else {                                                     \
        printf("FAIL\n");                                        \
        tests_failed++;                                          \
    }                                                            \
} while (0)

#endif
