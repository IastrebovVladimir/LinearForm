#ifndef LINEARFORM_H
#define LINEARFORM_H

#include "types.h"
#include <stddef.h>


typedef struct _linely_form{
    size_t n;
    void *coeffs;
    const TypeInfo *type;
    char **operations;
} LinearForm;


LinearForm *createForm(const TypeInfo *coeffs_type, size_t n);
void deleteForm(LinearForm *line_form);
void setcoeff(LinearForm *line_form, size_t i, void *new_coeff);
LinearForm *addition(const LinearForm* line_form1, const LinearForm* line_form2); // сделать отдельное кольцо на тесты 2+3 a+b
LinearForm *subtraction(const LinearForm* line_form1, const LinearForm* line_form2);
void scalar(LinearForm* line_form, void* factor);
void valueForm(const LinearForm* line_form, const void* args, size_t n_args, void* result); // операции с указателями, переложить на main.c
// сделать 2-ю реализацию
#endif