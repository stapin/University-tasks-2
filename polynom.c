#include "stdio.h"
#include "stdlib.h"

typedef struct poly
{
    int coeff;
    int exp;
    struct poly *next;
} poly;



void poly_free(poly *head)
{
    if (!head) 
    {
        puts("Warning: try to free NULL. [poly_free]");
        return;
    }
    poly *curr = head;
    poly *next;
    while (curr->next)
    {
        next = curr->next;
        free(curr);
        curr = next;   
    }
    return;
}



// Добавить функцию merge(poly*, int coeff, int exp);
// Добавляет в отсортированный список мономов.

int merge(poly **head, int coeff, int exp)
{
    if (!head) return 0;
    if (!coeff) return 1;
    if (!(*head))
    {
        *head = malloc(sizeof(poly));
        (*head)->coeff = coeff;
        (*head)->exp = exp;
        (*head)->next = NULL;
        return 1;
    }
    
    if ((*head)->exp > exp)
    {
        poly *tmp = *head;
        *head = malloc(sizeof(poly));
        (*head)->coeff = coeff;
        (*head)->exp = exp;
        (*head)->next = tmp;
        return 1;
    }
    

    poly *curr = *head;
    while (curr->next)
    {
        if (curr->next->exp <= exp) curr = curr->next; // => (curr->exp <= exp)
        // (next->exp > exp) для остальных условий.
        else if (curr->exp == exp)
        {
            // coeff = 0?
            curr->coeff += coeff;
            return 1;
        }
        else if (curr->exp < exp)
        {
            poly *tmp = curr->next;
            curr->next = malloc(sizeof(poly));
            curr->next->coeff = coeff;
            curr->next->exp = exp;
            curr->next->next = tmp;
            return 1;
        }
    }
    // curr->exp <= exp
    if (curr->exp == exp)
    {
        // Добавить случай, когда coeff = 0;
        curr->coeff += coeff;
        return 1;
    }
    curr->next = malloc(sizeof(poly));
    curr = curr->next;
    curr->coeff = coeff;
    curr->exp = exp;
    curr->next = NULL;
    return 1;
}

// version 2.
// add macros for states.
poly *poly_get(const char *str)
{
    poly *result = NULL;
    // poly *curr = result;
    const char *p = str;
    int curr_coeff = 0;
    int curr_exp = 0;
    int sign = 1;
    int state = 1;
    char *end = NULL;
    while (*p)
    {
        if (*p == ' ')
        {
            ++p;
            continue;
        }
        if (state == 1)
        {
            curr_coeff = strtol(p, &end, 0) * sign;
            p = end;
            if (!curr_coeff)
            {
                printf("%s\n", "Wrong input. State 1, expected number.");
                poly_free(result);
                return NULL;
            }
            ++state;
            continue;
        }
        if (state == 2)
        {
            if (*p == 'x')
            {
                ++p;
                ++state;
                continue;
            }
            else
            {
                // printf("%s\n", "Error state 2!");
                // return NULL;
                curr_exp = 0;
                merge(&result, curr_coeff, curr_exp);
                state = 5;
                continue;
            }
        }
        if (state == 3)
        {
            if (*p == '^')
            {
                ++p;
                ++state;
            }
            else
            {
                curr_exp = 1;
                merge(&result, curr_coeff, curr_exp);
                state = 5;
                // printf("%s\n", "Error state 3!");
                // return NULL;
            }
            continue;
        }
        if (state == 4)
        {

            curr_exp = strtol(p, &end, 0);
            p = end;
            if (!curr_exp)
            {
                printf("%s\n", "Wrong input. State 4, expected number.");
                poly_free(result);
                return NULL;
            }
            merge(&result, curr_coeff, curr_exp);
            ++state;
            continue;
        }
        if (state == 5)
        {
            if (*p == '-' || *p == '+')
            {
                sign = 44 - *p;
                state = 1;
                ++p;
                continue;
            }
            // puts("Wrong input, state 5. Exepted sign.");
            // poly_free(result);
            // return NULL;
            state = 1;
            continue;
        }
        else
        {
            puts("Unknown error.");
            poly_free(result);
            return NULL;
        }
    }

    if (state != 5 && state != 1 && state != 2)
    {
        printf("%s\n", "Error, expected state 5. Wrong input.");
        return NULL;
    }
    if (state == 2)
    {
        curr_exp = 0;
        merge(&result, curr_coeff, curr_exp);
    }
    
    return result;
}


poly *poly_add(const poly *a, const poly *b)
{
    if (!a || !b)
    {
        puts("poly_add error! arguments are NULL.");
        return NULL;
    }
    
    poly *result = malloc(sizeof(poly));
    poly *curr = result;
    const poly *p1 = a;
    const poly *p2 = b;
    do
    {
        if (p1->exp < p2->exp)
        {
            curr->exp = p1->exp;
            curr->coeff = p1->coeff;
            p1 = p1->next;
            
        }
        else if (p1->exp == p2->exp)
        {
            curr->exp = p1->exp;
            curr->coeff = p1->coeff + p2->coeff;
            p1 = p1->next;
            p2 = p2->next;
            
        }
        else if (p1->exp > p2->exp)
        {
            curr->exp = p2->exp;
            curr->coeff = p2->coeff;
            p2 = p2->next;
        }
        if (p1 || p2)
        {
            curr->next = malloc(sizeof(poly));
            curr = curr->next;
        }
    }
    while (p1 && p2);
    if (!p1 && p2)
    {
        while (p2)
        {
            curr->exp = p2->exp;
            curr->coeff = p2->coeff;
            p2 = p2->next;
            if (p2)
            {
                curr->next = malloc(sizeof(poly));
                curr = curr->next;
            }
        }
        curr->next = NULL;
        return result;
    }
    if (!p2 && p1)
    {
        while (p1)
        {
            curr->exp = p1->exp;
            curr->coeff = p1->coeff;
            p1 = p1->next;
            if (p1)
            {
                curr->next = malloc(sizeof(poly));
                curr = curr->next;
            }
        }
        curr->next = NULL;
        return result;
    }
    return result;
}



void poly_print(const poly *head)
{
    if (!head)
    {
        puts("Print error!");
        return;
    }
    const poly *curr = head;
    while (curr->next)
    {
        if (!curr->exp) printf("%d + ", curr->coeff);
        else if (curr->exp == 1) printf("%dx + ", curr->coeff);
        else printf("%dx^%d + ", curr->coeff, curr->exp);
        curr = curr->next;
    }
    if (!curr->exp) printf("%d\n", curr->coeff);
    else if (curr->exp == 1) printf("%dx\n", curr->coeff);
    else  printf("%dx^%d\n", curr->coeff, curr->exp);
}


poly *poly_mult(const poly *a, const poly *b)
{
    poly *result = NULL;
    const poly *pa = a, *pb = b;
    while (pa)
    {
        while (pb)
        {
            // проверка на нулевые коэффициенты.
            merge(&result, pa->coeff * pb->coeff, pa->exp + pb->exp);
            pb = pb->next;
        }
        pb = b;
        pa = pa->next;
    }
    return result;
}

void test_add()
{
    //char *p1 = "4x^2 + 5x^4 - 159x^8";
    char *p2 = "25x^1 - -14x^4 + 4x^1 --3 + 2x^100";
    poly *a = poly_get(p2);
    poly *b = poly_get("3x^2 - 1x^3 - 8x^4 + 158x^8 + 8 - 6x^13");
    poly_print(a);
    poly_print(b);
    poly *c = poly_add(a, b);
    poly_print(c);
    poly_free(c);
    poly_free(a);
    poly_free(b);

}

void test_mult()
{
    poly *a = poly_get("1 + 1x^1 + 1x^2 + 1x^3");
    poly *b = poly_get("1 + 1x^1 + 1x^2 - 1x^3");
    poly_print(a);
    poly_print(b);
    poly *c = poly_mult(a, b);
    poly_print(c);
}

int main()
{    
    //test_add();
    test_mult();
    return 0;
}