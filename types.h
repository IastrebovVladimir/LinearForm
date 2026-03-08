#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

typedef struct TypeInfo {
    size_t size;
    void (*sum) (const void*, const void*, void*);
    void (*sub) (const void*, const void*, void*);
    void (*neg) (void*);
    void (*mul) (const void*, const void*, void*);
    void (*zero) (void*);
    void (*print) (const void*);
    int (*scan) (void*);
    const char *(*to_string)(const void *arg);
} TypeInfo;

extern const TypeInfo *get_int_t(void);
extern const TypeInfo *get_double_t(void);

#endif
