#include <stdio.h>
#include <stdlib.h>
#include "menu.h"
#include "types.h"
#include "linearform.h"

void case_create_form(LinearForm **line_form, const TypeInfo **t, int *n) {
    int num_type;
    int n_local;

    if (*line_form) {
        printf("Form is already created\n");
        return;
    }
    printf("Choose data type:\n");
    printf("1. int\n");
    printf("2. double\n");
    while (!get_int_t()->scan(&num_type) || num_type < 1 || num_type > 2)
        printf("Error! Enter a digit from 1 to 2\n");

    printf("Enter the number of coefficients\n");
    while (!get_int_t()->scan(&n_local) || n_local <= 0)
        printf("Error! Enter the number of coefficients\n");

    if (num_type == 1)
        *t = get_int_t();
    else
        *t = get_double_t();

    *n = n_local;
    *line_form = createForm(*t, (size_t)*n);
}

void case_delete_form(LinearForm **line_form) {
    if (!*line_form) {
        printf("Form is not created\n");
        return;
    }
    deleteForm(*line_form);
    *line_form = NULL;
    printf("Form has been deleted\n");
}

void case_setcoeff(LinearForm *line_form, const TypeInfo *t) {
    int index;
    if (!line_form) {
        printf("Form is not created\n");
        return;
    }
    int n = (int)line_form->n;
    printf("Enter coefficient index from 0 to %d: ", n - 1);
    while (!get_int_t()->scan(&index) || index < 0 || index > n - 1)
        printf("Error! Enter a digit from 0 to %d: ", n - 1);
    printf("Enter the coefficient (it must be of the same type as the form): ");
    void *coeff = malloc(t->size);
    while (!t->scan(coeff))
        printf("Error! Enter the coefficient (it must be of the same type as the form): ");
    setcoeff(line_form, index, coeff);
    free(coeff);
}

void case_show_coeffs(LinearForm *line_form) {
    int index;
    if (!line_form) {
        printf("Form is not created\n");
        return;
    }
    void *elem;
    for (index = 0; index < (int)line_form->n; index++) {
        elem = (char*)line_form->coeffs + index * line_form->type->size;
        line_form->type->print(elem);
        if (index != (int)line_form->n - 1)
            printf(" ");
    }
    printf("\n");
}

void case_addition(LinearForm **line_form, const TypeInfo *t, int *n) {
    int num_coeffs_2, index;
    if (!*line_form) {
        printf("Form is not created\n");
        return;
    }
    printf("Enter the number of coefficients in the 2nd linear form\n");
    while (!get_int_t()->scan(&num_coeffs_2) || num_coeffs_2 <= 0)
        printf("Error! Enter the number of coefficients\n");

    LinearForm *line_form2 = createForm(t, (size_t)num_coeffs_2);
    void *coeff = malloc(t->size);
    for (index = 0; index < num_coeffs_2; index++) {
        printf("Enter coefficient (it must be of the same type as the form) %d: ", index);
        while (!t->scan(coeff))
            printf("Error! Enter coefficient (it must be of the same type as the form) %d: ", index);
        setcoeff(line_form2, index, coeff);
    }
    free(coeff);

    LinearForm *addForm = addition(*line_form, line_form2);
    deleteForm(*line_form);
    deleteForm(line_form2);
    *line_form = addForm;
    *n = (int)(*line_form)->n;
    printf("Addition completed\n");
}

void case_substraction(LinearForm **line_form, const TypeInfo *t, int *n) {
    int num_coeffs_2, index;
    if (!*line_form) {
        printf("Form is not created\n");
        return;
    }
    printf("Enter the number of coefficients in the 2nd linear form\n");
    while (!get_int_t()->scan(&num_coeffs_2) || num_coeffs_2 <= 0)
        printf("Error! Enter the number of coefficients\n");

    LinearForm *line_form2 = createForm(t, (size_t)num_coeffs_2);
    void *coeff = malloc(t->size);
    for (index = 0; index < num_coeffs_2; index++) {
        printf("Enter coefficient (it must be of the same type as the form) %d: ", index);
        while (!t->scan(coeff))
            printf("Error! Enter coefficient (it must be of the same type as the form) %d: ", index);
        setcoeff(line_form2, index, coeff);
    }
    free(coeff);

    LinearForm *subForm = subtraction(*line_form, line_form2);
    deleteForm(*line_form);
    *line_form = subForm;
    *n = (int)(*line_form)->n;
    deleteForm(line_form2);
    printf("Subtraction completed\n");
}

void case_scale_form(LinearForm *line_form, const TypeInfo *t) {
    if (!line_form) {
        printf("Form is not created\n");
        return;
    }
    void *multiplier = malloc(t->size);
    if (!multiplier) {
        printf("Memory allocation error\n");
        return;
    }
    printf("Enter scalar (it must be of the same type as the form): ");
    while (!t->scan(multiplier))
        printf("Error! Enter scalar (it must be of the same type as the form): ");
    scaleForm(line_form, multiplier);
    free(multiplier);
}

void case_evaluate_form(LinearForm *line_form, const TypeInfo *t) {
    int index;
    if (!line_form) {
        printf("Form is not created\n");
        return;
    }
    size_t n_local = line_form->n;
    printf("Enter variables (%zu variables): ", n_local);
    void *args = malloc(n_local * t->size);
    void *arg_i;
    if (!args) {
        printf("Memory allocation error\n");
        return;
    }

    for (index = 0; index < (int)n_local; index++) {
        arg_i = (char*)args + index * t->size;
        printf("Enter argument (it must be of the same type as the form) %d: ", index);
        while (!t->scan(arg_i))
            printf("Error! Enter argument (it must be of the same type as the form) %d: ", index);
    }

    void *result = malloc(t->size);
    if (!result) {
        free(args);
        printf("Memory allocation error\n");
        return;
    }

    evaluateForm(line_form, args, n_local, result);

    printf("Value is: ");
    t->print(result);
    printf("\n");
    free(result);
    free(args);
}

void case_operations(LinearForm *line_form) {
    int index;
    if (!line_form) {
        printf("Form is not created\n");
        return;
    }
    int n = (int)line_form->n;
    printf("Linear form: ");
    for (index = 0; index < n; index++) {
        if (line_form->operations && line_form->operations[index] && index == n - 1) {
            printf("(%s)x%d\n", line_form->operations[index], index);
        } else if (line_form->operations && line_form->operations[index]) {
            printf("(%s)x%d+", line_form->operations[index], index);
        } else if ((!line_form->operations || !line_form->operations[index]) && index != n - 1) {
            printf("(0)x%d+", index);
        } else if (!line_form->operations || !line_form->operations[index]) {
            printf("(0)x%d\n", index);
        }
    }

}



