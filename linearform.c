#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "linearform.h"

LinearForm *createForm(const TypeInfo *coeffs_type, size_t n) {
    LinearForm *line_form = malloc(sizeof(LinearForm));
    if (!line_form)
        return NULL;
    line_form->n = n;
    line_form->type = coeffs_type;
    line_form->coeffs = malloc(n * line_form->type->size);
    void  *elem;
    for (size_t index = 0; index < n; index++) {
        elem = (char*)line_form->coeffs + index * line_form->type->size;
        line_form->type->zero(elem);
    }
    line_form->operations = calloc(n, sizeof(char*));
    return line_form;
}

void deleteForm(LinearForm *line_form) {
    if (!line_form)
        return;
    if (line_form->operations) {
        for (size_t index = 0; index < line_form->n; index++)
            free(line_form->operations[index]);
        free(line_form->operations);
    }

    if (line_form->coeffs)
        free(line_form->coeffs);
    
    free(line_form);
}
void setcoeff(LinearForm *line_form, size_t index, void *new_coeff) {
    if (line_form && index < line_form->n) {
        void *elem = (char*)line_form->coeffs + index * line_form->type->size;
        memcpy(elem, new_coeff, line_form->type->size);
        const char *str = line_form->type->to_string(new_coeff);
        char *copy = malloc(strlen(str) + 1);
        if (!copy) {
            free((void*)str);      
            return;
        }
        memcpy(copy, str, strlen(str) + 1);
        free((void*)str);
        free(line_form->operations[index]);
        line_form->operations[index] = copy;
    }
}

static char *addition_op(const char *left_coeff, const char *right_coeff) // функция которая выдает операцию сложения left_coeff = a, right_coeff = b -> a+b
{
    // left_coeff - строка левого коэфф., right_coeff - строка 2 коэфф
    const char *left_coeff_safe = left_coeff ? left_coeff  : "";
    const char *right_coeff_safe = right_coeff ? right_coeff : "";

    size_t left_len  = strlen(left_coeff_safe); // длина левого коэффициента
    size_t right_len = strlen(right_coeff_safe); // длина правого коэффициента

    if (left_len == 0 && right_len == 0) //нет смысла создавать если они оба пустые
        return NULL;

    size_t added_len = (left_len && right_len ? 1 : 0) + right_len;
    // Если у нас есть левый и правый коэфф., то между ними нужен плюс

    char *result = malloc(left_len + added_len + 1);
    // result— результирующая строка вида "a+b"
    if (!result)
        return NULL;
    char *position = result;  // текущая позиция записи в result
    if (left_len) {
        memcpy(position, left_coeff_safe, left_len);  // копируем левый коэффициент
        position += left_len;
    }
    if (right_len) {
        if (left_len && right_coeff_safe[0] != '-') {
            // если есть левый коэфф и правый коэфф не начинается с '-', вставляем '+'
            *position = '+';
            position++; // оказываемся на начале правого коэфф.
        }
        memcpy(position, right_coeff_safe, right_len); // копируем правый коэффициент
        position += right_len;
    }

    *position = '\0'; // завершаем строку
    return result;
}

static char *subtraction_op(const char *left_coeff, const char *right_coeff) {
    // left_coeff = a, right_coeff = b -> a-b
    if (!left_coeff && !right_coeff)
        return NULL;

    const char *left_coeff_safe = left_coeff ? left_coeff  : "";
    const char *right_coeff_safe = right_coeff ? right_coeff : "";

    size_t left_len  = strlen(left_coeff_safe);
    size_t right_len = strlen(right_coeff_safe);

    if (left_len == 0 && right_len == 0)
        return NULL;

    // Строим -right_safe с перевёрнутыми знаками: a-b+c -> -a+b-c
    char *flipped_right = malloc(right_len + 2);
    if (!flipped_right)
        return NULL;

    size_t read_position  = 0;
    size_t write_position = 0;
    size_t scopes_open = 0; // количество открытых скобок, например ((a+b - 2 открытые скобки, ((a+b) - 1 открытая скобка, ((a+b)+c) - 0 открытых скобок

    if (right_len > 0 && right_coeff_safe[0] != '-') {
        flipped_right[write_position] = '-'; // нет минуса в начале: a -> -a
        write_position++;
    }

    for (; read_position < right_len; read_position++, write_position++) {
        char symbol = right_coeff_safe[read_position];
        if (symbol == '+' && scopes_open == 0) // - * + = - если у нас есть выражение типа (a+b) - То он проверяет кол-во открытых скобок и если уже scopes_open > 0, то скобки открыты.
            // (a+b) -> -(a+b), scopes_open нужен чтобы внутри скобок ничего не менять, иначе выражение будет неверным
            flipped_right[write_position] = '-';
        else if (symbol == '-' && scopes_open == 0) // - * - = +
            flipped_right[write_position] = '+';
        else {
            if (symbol == '(')
                scopes_open++;
            if (symbol == ')')
                scopes_open--;
            flipped_right[write_position] = symbol;
        }
    }

    flipped_right[write_position] = '\0';
    if (left_len == 0)
        return flipped_right; // если слева пусто, это просто "-flipped_right"

    // склеиваем: left_safe + flipped_right + \0
    char *result = malloc(left_len + strlen(flipped_right) + 1);
    if (!result) {
        free(flipped_right);
        return NULL;
    }
    memcpy(result, left_coeff_safe, left_len);
    memcpy(result + left_len, flipped_right, strlen(flipped_right) + 1);
    free(flipped_right);
    return result;
}

// функция которая выдает операцию скаляра, при коэффициенте a+b и скаляре k выдает k(a + b)
static char *scale_form_op(const char *line_form, const char *scalar_str) {
    if (!line_form || !scalar_str)
        return NULL;

    size_t form_len = strlen(line_form);
    size_t scalar_len = strlen(scalar_str);

    char *result = malloc(scalar_len + 1 + 1 + form_len + 1 + 1); // k*(form)\0
    if (!result)
        return NULL;
    size_t position = 0;
    memcpy(result + position, scalar_str, scalar_len);
    position += scalar_len;

    result[position] = '*';
    position++;
    result[position] = '(';
    position++;
    memcpy(result + position, line_form, form_len);
    position += form_len;
    result[position] = ')';
    position++;
    result[position] = '\0';

    return result;
}

LinearForm *addition(const LinearForm* line_form1, const LinearForm* line_form2) {
    if (!line_form1 || !line_form2 || line_form1->type != line_form2->type)
        return NULL;
    size_t num_min, num_max, index;
    const LinearForm *formbuffer;
    if (line_form1->n >= line_form2->n) {
        num_max = line_form1->n;
        num_min = line_form2->n;
        formbuffer = line_form1;
    } else {
        num_max = line_form2->n;
        num_min = line_form1->n;
        formbuffer = line_form2;
    } // вычисляем форму с мин. и макс. кол-вом коэффициентов
    LinearForm *newForm = createForm(line_form1->type, num_max);
    void *elem1, *elem2, *elem_new;
    for (index = 0; index < num_min; index++) { // проходим от 0 элемента до num_min не включительно, Так как тут будут коэффициенты у 1 и 2 формы
        elem1 = (char*)line_form1->coeffs + index * line_form1->type->size; // элемент 1 формы
        elem2 = (char*)line_form2->coeffs + index * line_form1->type->size; // элемент 2 формы
        elem_new = (char*)newForm->coeffs + index * line_form1->type->size; // элемент суммирующей формы
        newForm->type->sum(elem1, elem2, elem_new); // записываем в elem_new сумму 2 элементов

        if (newForm->operations && line_form1->operations && line_form2->operations) {
            char *temp = addition_op(line_form1->operations[index],line_form2->operations[index]); // записываем в temp операцию сложения 1 и 2 формы
            if (temp) {
                free(newForm->operations[index]); // освобождаем операции в newForm
                newForm->operations[index] = temp; // записываем туда temp
            }
        }

    }
    for (index = num_min; index < num_max; index++) {
        elem1 = (char*)formbuffer->coeffs + index * line_form1->type->size;
        elem_new = (char*)newForm->coeffs + index * line_form1->type->size;
        memcpy(elem_new, elem1, newForm->type->size); // от num_min до num_max коэффициентов у одной из форм не будет, поэтому просто копируем элемент и потом историю
        if (newForm->operations && formbuffer->operations) {
            free(newForm->operations[index]);
            if (formbuffer->operations[index])
                newForm->operations[index] = strdup(formbuffer->operations[index]);
            else
                newForm->operations[index] = NULL;
        }

    }
    return newForm;
}

LinearForm *subtraction(const LinearForm* line_form1, const LinearForm* line_form2) {
    if (!line_form1 || !line_form2 || line_form1->type != line_form2->type)
        return NULL;
    size_t num_min, num_max, index;
    const LinearForm *formbuffer;
    if (line_form1->n >= line_form2->n) {
        num_max = line_form1->n;
        num_min = line_form2->n;
        formbuffer = line_form1;
    } else {
        num_max = line_form2->n;
        num_min = line_form1->n;
        formbuffer = line_form2;
    } // вычисляем форму с мин. и макс. кол-вом коэффициентов
    LinearForm *newForm = createForm(line_form1->type, num_max);
    void *elem1, *elem2, *elem_new;
    for (index = 0; index < num_min; index++) {
        elem1 = (char*)line_form1->coeffs + index * line_form1->type->size; // элемент 1 формы
        elem2 = (char*)line_form2->coeffs + index * line_form1->type->size; // элемент 2 формы
        elem_new = (char*)newForm->coeffs + index * line_form1->type->size; // элемент разности форм
        newForm->type->sub(elem1, elem2, elem_new); // записываем в elem_new разность 2 элементов

        if (newForm->operations && line_form1->operations && line_form2->operations) {
            char *temp = subtraction_op(line_form1->operations[index],line_form2->operations[index]); // записываем в temp операцию разности 1 и 2 формы
            if (temp) {
                free(newForm->operations[index]); // освобождаем историю операций в newForm
                newForm->operations[index] = temp; // записываем туда temp
            }
        }

    }
    if (formbuffer == line_form1) //если у 1 формы больше коэффициентов то мы просто копируем в operations новой формы историю операций 1 формы
        for (index = num_min; index < num_max; index++) {
            elem1 = (char *) formbuffer->coeffs + index * line_form1->type->size;
            elem_new = (char *) newForm->coeffs + index * line_form1->type->size;
            memcpy(elem_new, elem1, newForm->type->size); // от num_min до num_max коэффициентов у одной из форм не будет, поэтому просто копируем элемент и потом историю

            if (newForm->operations && formbuffer->operations) {
                free(newForm->operations[index]);
                if (formbuffer->operations[index])
                    newForm->operations[index] = strdup(formbuffer->operations[index]);
                else
                    newForm->operations[index] = NULL;
            }
        }
    else { // если у 2 формы больше коэффициентов, то нам нужно поменять в операциях 2 формы плюс на минус и наоборот
        for (index = num_min; index < num_max; index++) {
            elem1 = (char *)formbuffer->coeffs + index * line_form1->type->size;
            elem_new = (char *)newForm->coeffs + index * line_form1->type->size;
            memcpy(elem_new, elem1, line_form1->type->size);  // от num_min до num_max коэффициентов у одной из форм не будет, поэтому просто копируем элемент
            newForm->type->neg(elem_new); // после копирования элемента умножаем его на -1

            if (newForm->operations && formbuffer->operations) {
                free(newForm->operations[index]);
                if (formbuffer->operations[index])
                    newForm->operations[index] = subtraction_op(NULL, formbuffer->operations[index]); //если есть операции в formbuffer, то в operations записываем преобразованную операцию formbuffer
                else
                    newForm->operations[index] = NULL;
            }
        }
    }
    return newForm;
}

void scaleForm(LinearForm* line_form, void* factor) { // функция, умножающая все коэффициенты формы на скаляр k: a1, a2, ..., an -> k*a1, k*a2, ..., k*an
    if (line_form) {
        void *elem;
        const char* scalar_str = line_form->type->to_string(factor);
        for (size_t index = 0; index < line_form->n; index++) {
            elem = (char*)line_form->coeffs + index * line_form->type->size;
            line_form->type->mul(elem, factor, elem);
            if (line_form->operations && line_form->operations[index]) {
                char *new_str = scale_form_op(line_form->operations[index], scalar_str);
                if (new_str) {
                    free(line_form->operations[index]);
                    line_form->operations[index] = new_str;
                } else {
                    free(line_form->operations[index]);
                    line_form->operations[index] = NULL;
                 }
            }
        }
        free((void*)scalar_str);
    }
}

void evaluateForm(const LinearForm* line_form, const void* args, size_t n_args, void* result) {
    // вычисляет значение линейной формы a1x1 + a2x2 + ... + anxn при подстановке t1, t2, ..., tn из args вместо x1, x2, ..., xn
    if (!line_form || !args || line_form->n != n_args)
        return;
    void* factor = malloc(line_form->type->size);
    if (!factor)
        return;
    line_form->type->zero(result);
    void *elem, *arg_i;
    for (size_t index = 0; index < line_form->n; index++) {
        elem = (char*)line_form->coeffs + index * line_form->type->size;
        arg_i = (char*)args + index * line_form->type->size;
        line_form->type->mul(elem, arg_i, factor);
        line_form->type->sum(result, factor, result);
    }
    free(factor);
}
