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
    const char *left_coeff_safe;
    if (left_coeff)
        left_coeff_safe = left_coeff;
    else
        left_coeff_safe = "";

    const char *right_coeff_safe;
    if (right_coeff)
        right_coeff_safe = right_coeff;
    else
        right_coeff_safe = "";

    size_t left_len  = strlen(left_coeff_safe); // длина левого коэффициента
    size_t right_len = strlen(right_coeff_safe); // длина правого коэффициента

    if (left_len == 0 && right_len == 0) //нет смысла создавать если они оба пустые
        return NULL;

    size_t added_len = (left_len && right_len ? 1 : 0) + right_len;
    // Если у нас есть левый и правый коэфф., то между ними нужен плюс

    char *result_str = malloc(left_len + added_len + 1);
    // result_str — результирующая строка вида "a+b"
    if (!result_str)
        return NULL;

    char *write_pos = result_str;  // текущая позиция записи в result_str

    if (left_len) {
        memcpy(write_pos, left_coeff_safe, left_len);  // копируем левый коэффициент
        write_pos += left_len;
    }
    if (right_len) {
        if (left_len && right_coeff_safe[0] != '-') {
            // если есть левый коэфф и правый коэфф не начинается с '-', вставляем '+'
            *write_pos = '+';
            write_pos++; // оказываемся на начале правого коэфф.
        }
        memcpy(write_pos, right_coeff_safe, right_len); // копируем правый коэффициент
        write_pos += right_len;
    }

    *write_pos = '\0'; // завершаем строку
    return result_str;
}


static char *subtraction_op(const char *left_coeff, const char *right_coeff) // функция которая выдает операцию вычитания left_coeff = a, right_coeff = b -> a-b
{
    // left_coeff - строка левого коэфф., right_coeff - строка 2 коэфф
    const char *left_coeff_safe;
    if (left_coeff)
        left_coeff_safe = left_coeff;
    else
        left_coeff_safe = "";

    const char *right_coeff_safe;
    if (right_coeff)
        right_coeff_safe = right_coeff;
    else
        right_coeff_safe = "";

    size_t left_len  = strlen(left_coeff_safe);   // длина левого коэффициента
    size_t right_len = strlen(right_coeff_safe);  // длина правого коэффициента

    if (left_len == 0 && right_len == 0) // нет смысла создавать, если оба пустые
        return NULL;

    char *flipped_right = malloc(right_len + 1); // правый коэффициент с измененными знаками
    if (!flipped_right)
        return NULL;

    size_t index; // индекс в цикле по символам right_coeff_safe
    for (index = 0; index < right_len; index++) {
        if (right_coeff_safe[index] == '+') // - * + = -
            flipped_right[index] = '-';
        else if (right_coeff_safe[index] == '-') //- * - = +
            flipped_right[index] = '+';
        else
            flipped_right[index] = right_coeff_safe[index]; //если это не знак +/-, записываем как есть
    }
    flipped_right[right_len] = '\0'; // завершаем строку

    if (right_len > 0 && flipped_right[0] != '-' && flipped_right[0] != '+') {
        // если длина правого коэфф > 0 и первый элемент не +/-, то выражение a -> -a
        memmove(flipped_right + 1, flipped_right, right_len + 1);
        flipped_right[0] = '-';
        right_len++;
    }

    char *result_str = malloc(left_len + right_len + 1); // результирующая строка "left - right"
    if (!result_str) {
        free(flipped_right);
        return NULL;
    }

    char *write_pos = result_str; // позиция записи в итоговой строке

    if (left_len > 0) {
        memcpy(write_pos, left_coeff_safe, left_len); // записываем левый коэффициент
        write_pos += left_len;
    }

    memcpy(write_pos, flipped_right, right_len); // записываем обновленный правый
    write_pos += right_len;

    *write_pos = '\0'; // завершаем строку

    free(flipped_right);
    return result_str;
}


static char *neg_op(const char *str) // функция которая превращает из a -> -a(или -a->a) в operations
{
    // str - строка коэфф
    const char *str_safe;
    if (str)
        str_safe = str;
    else
        str_safe = "";
    size_t str_len = strlen(str_safe); // длина коэффициента

    if (str_len == 0) // нет смысла создавать, если строка пустая
        return NULL;

    char *flipped = malloc(str_len + 1); // коэффициент с измененными знаками
    if (!flipped)
        return NULL;

    size_t index; // индекс в цикле по символам str_safr
    for (index = 0; index < str_len; index++) {
        if (str_safe[index] == '+') // - * + = -
            flipped[index] = '-';
        else if (str_safe[index] == '-') // - * - = +
            flipped[index] = '+';
        else
            flipped[index] = str_safe[index]; //если это не знак +/-, записываем как есть
    }
    flipped[str_len] = '\0'; //

    if (str_len > 0 && flipped[0] != '-' && flipped[0] != '+') {
        // если длина правого коэфф > 0 и первый элемент не +/-, то выражение a -> -a
        memmove(flipped + 1, flipped, str_len + 1);
        flipped[0] = '-';
        str_len++;
    } else if (str_len > 0 && flipped[0] == '+') {
        // если строка начинается с '+', убираем ведущий плюс: "+a" -> "a"
        memmove(flipped, flipped + 1, str_len);
        str_len--;
    }

    char *result = malloc(str_len + 2); // результирующая строка "-str"
    if (!result) {
        free(flipped);
        return NULL;
    }

    char *write_pos = result; // позиция записи в итоговой строке

    memcpy(write_pos, flipped, str_len); // записываем обновленный коэффициент
    write_pos += str_len;

    *write_pos = '\0'; // заканчиваем строку

    free(flipped);
    return result;
}
// функция которая выдает операцию скаляра, при коэффициенте a+b и скаляре k выдает k*a + k*b
static char *scalar_op(const char *line_form, const char *scalar_str) {
    if (!line_form || !scalar_str)
        return NULL;
    //line_form - строка коэфф. лин. формы, scalar+ptr
    size_t form_len = strlen(line_form);
    size_t scalar_len = strlen(scalar_str);

    char *scalar_safe;
    size_t scalar_safe_len;

    if (scalar_len > 0 && scalar_str[0] == '-') {
        scalar_safe_len = scalar_len + 2; // '(' + scalar + ')'
        scalar_safe = malloc(scalar_safe_len + 1);   // +1 под '\0'
        if (!scalar_safe)
            return NULL;

        scalar_safe[0] = '('; //ставим скобки слева от минуса
        memcpy(scalar_safe + 1, scalar_str, scalar_len);
        scalar_safe[scalar_len + 1] = ')'; // ставим скобки справа от коэфф
        scalar_safe[scalar_safe_len] = '\0'; //конец строки
    } else {
        scalar_safe = (char*)scalar_str; // если минуса нет, оставляем строку как есть
        scalar_safe_len = scalar_len;
    }


    char *result = malloc(form_len * (scalar_safe_len + 3) + 1); // запас по длине
    if (!result) {
        if (scalar_safe != scalar_str)
            free(scalar_safe);
        return NULL;
    }

    size_t index_form = 0; //индекс по форме
    size_t index_result = 0; //индекс по результирующей строке

    // если первый символ это не \0+-*
    if (line_form[0] != '\0' && line_form[0] != '+' && line_form[0] != '-' &&line_form[0] != '*') {
        memcpy(result + index_result, scalar_safe, scalar_safe_len); // записываем в начало скаляр
        index_result += scalar_safe_len; //увеличиваем индекс на длину скаляра
        result[index_result] = '*'; // в получившемся индекс записываем *
        index_result++;
        while (line_form[index_form] != '\0' && line_form[index_form] != '+' && line_form[index_form] != '-' &&
               line_form[index_form] != '*') {
            result[index_result] = line_form[index_form];
            //далее пока не встретим +-*\0 и копируем элементы и увеличиваем индексы
            index_result++;
            index_form++;
        }
    }


    while (line_form[index_form] != '\0') { //идем до конца строки
        char elem = line_form[index_form];
        if (elem == '-' && index_form > 0 && line_form[index_form - 1] == '(') {
            // если sign это минус, то скаляр тут не нужен, поэтому идем дальше
            result[index_result] = elem;
            index_result++;
            index_form++;
            continue;
        }

        result[index_result] = elem; // копируем элемент в результирующую строку и увеличиваем индексы
        index_result++;
        index_form++;

        if (elem == '+' || elem == '-') { //если elem это +-, то проверяем чтобы следуюший элемент(мы увеличили индекс) не +-*\0
            if (line_form[index_form] != '\0' && line_form[index_form] != '+' && line_form[index_form] != '-' &&
                line_form[index_form] != '*') {
                memcpy(result + index_result, scalar_safe, scalar_safe_len); //записываем скаляр
                index_result += scalar_safe_len; // увеличиваем индекс на длину строки
                result[index_result] = '*'; // в получившемся индекс записываем *
                index_result++;

                while (line_form[index_form] != '\0' && line_form[index_form] != '+' && line_form[index_form] != '-' &&
                       line_form[index_form] != '*') {
                    result[index_result] = line_form[index_form];
                    index_result++;
                    index_form++;
                    //далее пока не встретим +-*\0 и копируем элементы и увеличиваем индексы
                }
            }
        }

    }

    result[index_result] = '\0'; // заканчиваем строку

    if (scalar_safe != scalar_str)
        free(scalar_safe);

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
                    newForm->operations[index] = neg_op(formbuffer->operations[index]); //если есть операции в formbuffer, то в operations записываем преобразованную операцию formbuffer
                else
                    newForm->operations[index] = NULL;
            }
        }
    }
    return newForm;
}

void scalar(LinearForm* line_form, void* factor) {
    if (line_form) {
        void *elem;
        const char* scalar_str = line_form->type->to_string(factor);
        for (size_t index = 0; index < line_form->n; index++) {
            elem = (char*)line_form->coeffs + index * line_form->type->size;
            line_form->type->mul(elem, factor, elem);
            if (line_form->operations && line_form->operations[index]) {
                char *new_str = scalar_op(line_form->operations[index], scalar_str);
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

void valueForm(const LinearForm* line_form, const void* args, size_t n_args, void* result) {
    if (!line_form || !args || line_form->n != n_args)
        return;
    void* factor = malloc(line_form->type->size);
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
