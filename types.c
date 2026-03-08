#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void sumInt(const void *arg1, const void *arg2, void* result) {
    *(int*)result = *(int*)arg1 + *(int*)arg2;
}

void sumDouble(const void *arg1, const void *arg2, void* result) {
    *(double*)result = *(double*)arg1 + *(double*)arg2;
}

void subInt(const void *arg1, const void *arg2, void* result) {
    *(int*)result = *(int*)arg1 - *(int*)arg2;
}

void subDouble(const void *arg1, const void *arg2, void* result) {
    *(double*)result = *(double*)arg1 - *(double*)arg2;
}

void mulInt(const void *arg1, const void *arg2, void* result) {
    *(int*)result = *(int*)arg1 * *(int*)arg2;
}

void mulDouble(const void *arg1, const void *arg2, void* result) {
    *(double*)result = *(double*)arg1 * *(double*)arg2;
}

void negInt(void *arg) {
    *(int*)arg = -*(int*)arg;
}

void negDouble(void *arg) {
    *(double*)arg = -*(double*)arg;
}
void zeroInt(void *arg) {
    *(int*)arg = 0;
}
void zeroDouble(void *arg) {
    *(double*)arg = 0.0;
}
void printInt(const void *arg) {
    printf("%d", *(const int*)arg);
}

void printDouble(const void *arg) {
    printf("%g", *(const double*)arg);
}

int scanInt(void *arg) {
    int tmp;
    int c;

    if (scanf("%d", &tmp) != 1) {
        while ((c = getchar()) != '\n' && c != EOF);
        return 0;
    }

    while ((c = getchar()) != '\n' && c != EOF) {
        if (!isspace(c)) {
            while (c != '\n' && c != EOF)
                c = getchar();
            return 0;
        }
    }

    *(int*)arg = tmp;
    return 1;
}

int scanDouble(void *arg) {
    double tmp;
    int c;

    if (scanf("%lf", &tmp) != 1) {
        while ((c = getchar()) != '\n' && c != EOF);
        return 0;
    }

    while ((c = getchar()) != '\n' && c != EOF) {
        if (!isspace(c)) {
            while (c != '\n' && c != EOF)
                c = getchar();
            return 0;
        }
    }

    *(double*)arg = tmp;
    return 1;
}



const char *int_to_string(const void *arg) {
    const int *v = arg;
    char buf[128];
    int len = snprintf(buf, sizeof(buf), "%d", *v);
    if (len < 0)
        return NULL;
    char *str = malloc((size_t)len + 1);
    if (!str)
        return NULL;
    memcpy(str, buf, (size_t)len + 1);
    return str;
}

const char *double_to_string(const void *arg) {
    const double *v = arg;
    char buf[128];
    int len = snprintf(buf, sizeof(buf), "%g", *v);
    if (len < 0)
        return NULL;
    char *str = malloc((size_t)len + 1);
    if (!str)
        return NULL;
    memcpy(str, buf, (size_t)len + 1);
    return str;
}



const TypeInfo *get_int_t(void) {
    static TypeInfo int_t;
    static int inited = 0;

    if (!inited) {
        int_t.size = sizeof(int);
        int_t.sum  = sumInt;
        int_t.sub  = subInt;
        int_t.neg  = negInt;
        int_t.mul  = mulInt;
        int_t.zero = zeroInt;
        int_t.print = printInt;
        int_t.scan = scanInt;
        int_t.to_string = int_to_string;
        inited = 1;
    }
    return &int_t;
}

const TypeInfo *get_double_t(void) {
    static TypeInfo double_t;
    static int inited = 0;

    if (!inited) {
        double_t.size = sizeof(double);
        double_t.sum  = sumDouble;
        double_t.sub  = subDouble;
        double_t.neg  = negDouble;
        double_t.mul  = mulDouble;
        double_t.zero = zeroDouble;
        double_t.print = printDouble;
        double_t.scan = scanDouble;
        double_t.to_string = double_to_string;
        inited = 1;
    }
    return &double_t;
}