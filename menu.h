#ifndef MENU_H
#define MENU_H
#include "linearform.h"
#include "types.h"

void case_create_form(LinearForm **line_form, const TypeInfo **t, int *n);
void case_delete_form(LinearForm **line_form);
void case_setcoeff(LinearForm* line_form, const TypeInfo *t);
void case_show_coeffs(LinearForm *line_form);
void case_addition(LinearForm **line_form, const TypeInfo *t, int *n);
void case_substraction(LinearForm **line_form, const TypeInfo *t, int *n);
void case_scalar(LinearForm* line_form, const TypeInfo *t);
void case_evaluate_form(LinearForm *line_form, const TypeInfo *t);
void case_operations(LinearForm* line_form);


#endif
