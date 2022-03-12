#include "stdio.h"
#include "stdlib.h"
#define NO_SUCH_ELEMENT -3
#define INCORRECT_ARG -2
#define WRONG_INPUT -1
#define UNKNOWN_ERROR 0
#define SUCCEED 1


typedef struct poly
{
    int coeff;
    int exp;
    struct poly *next;
} poly;

typedef struct exception
{
    int code;
    char *message;
} exception;

int poly_del_monomial(poly **head, int exp)
{
    if (!head) return INCORRECT_ARG;
    if (!(*head)) return NO_SUCH_ELEMENT;
    poly *prev = NULL;
    poly *curr = *head;
    while (curr)
    {
        if (curr->exp == exp)
        {
            if (prev)
            {
                prev->next = curr->next;
                free(curr);
                return 1;
            }
            *head = curr->next;
            free(curr);
            return 1;
        }
        prev = curr;
        curr = curr->next;   
    }
    return NO_SUCH_ELEMENT;
}

poly *poly_get_monomial(int c, int e) {
    poly *head = malloc(sizeof(poly));
    head->coeff = c;
    head->exp = e;
    head->next = NULL;
    return head;
}

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

int poly_add_monomial(poly **head, int coeff, int exp)
{
    if (!head) return 0;
    if (!coeff) return 1;
    if (!(*head))
    {
        *head = poly_get_monomial(coeff, exp);
        return 1;
    }
    if ((*head)->exp > exp)
    {
        poly *tmp = *head;
        *head = poly_get_monomial(coeff, exp);
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
            curr->coeff += coeff;
            if (!curr->coeff) poly_del_monomial(head, exp);
            return 1;
        }
        else if (curr->exp < exp)
        {
            poly *tmp = curr->next;
            curr->next = poly_get_monomial(coeff, exp);
            curr->next->next = tmp;
            return 1;
        }
    }
    // curr->exp <= exp
    if (curr->exp == exp)
    {
        curr->coeff += coeff;
        if (!curr->coeff) poly_del_monomial(head, exp);
        return 1;
    }
    curr->next = poly_get_monomial(coeff, exp);
    return 1;
}


int poly_get(const char *str, poly **result)
{
    *result = NULL;
    const char *p = str;
    int curr_coeff = 0;
    int curr_exp = 0;
    int sign = 1;
    int state = 0;
    char *end = NULL;
    while (*p)
    {
        if (*p == ' ')
        {
            ++p;
            continue;
        }
        if (state == 0)
        {
            if (*p == '-' || *p == '+')
            {
                if (*p == '-') sign = -sign;
                ++p;
                continue;
            }
            state = 1;
            continue;
        }
        if (state == 1)
        {
            curr_coeff = strtol(p, &end, 0) * sign;
            p = end;
            if (!curr_coeff)
            {
                printf("%s\n", "Wrong input. State 1, expected number.");
                poly_free(*result);
                return WRONG_INPUT;
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
                curr_exp = 0;
                poly_add_monomial(result, curr_coeff, curr_exp);
                state = 0;
                sign = 1;
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
                poly_add_monomial(result, curr_coeff, curr_exp);
                state = 0;
                sign = 1;
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
                poly_free(*result);
                return WRONG_INPUT;
            }
            poly_add_monomial(result, curr_coeff, curr_exp);
            state = 0;
            sign = 1;
            continue;
        }
        else
        {
            puts("Unknown error.");
            poly_free(*result);
            return UNKNOWN_ERROR;
        }
    }

    if (state != 0 && state != 1 && state != 2)
    {
        printf("%s\n", "Wrong input.");
        poly_free(*result);
        return WRONG_INPUT;
    }
    if (state == 2)
    {
        curr_exp = 0;
        poly_add_monomial(result, curr_coeff, curr_exp);
    }
    
    return SUCCEED;
}


poly *poly_add(const poly *a, const poly *b)
{
    if (!a || !b)
    {
        puts("poly_add error! arguments are NULL.");
        return NULL;
    }
    poly *result = NULL;
    poly **curr = &result;
    do
    {
        if (a->exp < b->exp)
        {
            poly_add_monomial(curr, a->coeff, a->exp);
            a = a->next;
        }
        else if (a->exp == b->exp)
        {
            poly_add_monomial(curr, a->coeff + b->coeff, a->exp);
            a = a->next;
            b = b->next;
        }
        else if (a->exp > b->exp)
        {
            poly_add_monomial(curr, b->coeff, b->exp);
            b = b->next;
        }
        curr = &((*curr)->next);
    }
    while (a && b);
    
    while (b)
    {
        poly_add_monomial(curr, b->coeff, b->exp);
        curr = &((*curr)->next);
        b = b->next;
    }
    while (a)
    {
        poly_add_monomial(curr, a->coeff, a->exp);
        curr = &((*curr)->next);
        a = a->next;
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
            poly_add_monomial(&result, pa->coeff * pb->coeff, pa->exp + pb->exp);
            pb = pb->next;
        }
        pb = b;
        pa = pa->next;
    }
    return result;
}

void test_add2()
{
    char *p2 = "- + --25  x  ^ 1 - -14x^4 + 2 + 4x^1 --3 + 2x^100";
    poly *a = NULL, *b = NULL;
    poly_get(p2, &a);
    poly_get("3x^2 - 1x^3 - 8x^4 + 158x^8 + 8 - 6x^13", &b);
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
    poly *a, *b;
    poly_get("1 + 1x^1 + 1x^2 + 1x^3", &a);
    poly_get("1 + 1x^1 + 1x^2 - 1x^3", &b);
    poly_print(a);
    poly_print(b);
    poly *c = poly_mult(a, b);
    poly_print(c);
}

int main()
{    
    test_add2();
    //test_mult();
    return 0;
}