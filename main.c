#include <stdio.h>
#include "linearform.h"
#include "types.h"
#include "menu.h"

int main(void) {
    int n = 0;
    int index_LF;
    LinearForm *line_form = NULL;
    const TypeInfo *t = NULL;

    printf("=== Linear forms ===\n");
    printf("1. Create a linear form\n");
    printf("2. Delete a linear form\n");
    printf("3. Set a coefficient\n");
    printf("4. Show form coefficients\n");
    printf("5. Add two forms (addition)\n");
    printf("6. Subtract forms (subtraction)\n");
    printf("7. Multiply by a scalar (scalar)\n");
    printf("8. Evaluate the form at a point (valueForm)\n");
    printf("9. Show linear form as expression\n");
    printf("0. Exit\n");

    while (1) {
        printf("Choose operation: ");
        if (!get_int_t()->scan(&index_LF)) {
            printf("Error! Enter a digit from 0 to 9\n");
            continue;
        }

        switch (index_LF) {
            case 0:
                printf("Exit\n");
                if (line_form) {
                    deleteForm(line_form);
                    line_form = NULL;
                }
                return 0;

            case 1:
                case_create_form(&line_form, &t, &n);
                break;

            case 2:
                case_delete_form(&line_form);
                break;

            case 3:
                case_setcoeff(line_form, t);
                break;

            case 4:
                case_show_coeffs(line_form);
                break;

            case 5:
                case_addition(&line_form, t, &n);
                break;

            case 6:
                case_substraction(&line_form, t, &n);
                break;

            case 7:
                case_scale_form(line_form, t);
                break;

            case 8:
                case_evaluate_form(line_form, t);
                break;

            case 9:
                case_operations(line_form);
                break;

            default:
                printf("No such operation! Enter a digit from 0 to 9\n");
                break;
        }
    }
}
