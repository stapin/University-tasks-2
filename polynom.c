#include "stdio.h"
#include "stdlib.h"

typedef struct poly
{
    int coeff;
    int exp;
    struct poly *next;
} poly;

// poly *poly_get(const char *str)
// {
//     poly *result = malloc(sizeof(poly));
//     poly *curr = result;
//     char *p = str;
//     int sign = 1;
//     int state = 1;
//     do
//     {
//         // добавить проверки на конец строки.
//         if (*p == ' ')
//         {
//             p++;
//             continue;
//         }
//         if (state == 1)
//         {
//             curr->coeff = strtol(p, &p, 0) * sign;
//             ++state;
//         }
//         if (state == 2)
//         {
//             if (*p == 'x')
//             {
//                 ++p;
//                 ++state;
//             }
//             else
//             {
//                 printf("%s\n", "Error state 2!");
//                 return NULL;
//             }
//         }
//         if (state == 3)
//         {
//             if (*p == '^')
//             {
//                 ++p;
//                 ++state;
//             }
//             else
//             {
//                 printf("%s\n", "Error state 3!");
//                 return NULL;
//             }
//         }
//         if (state == 4)
//         {
//             curr->exp = strtol(p, &p, 0);
//             ++state;
//         }
//         while (*p == ' ') ++p;
//         if (*p == '-' || *p == '+')
//         {
//             curr->next = malloc(sizeof(poly));
//             curr = curr->next;
//             sign = 44 - *p;
//             state = 1;
//             ++p;
//         }
//     }
//     while (*p);
//     curr->next = NULL;
//     return result;
// }


// 3й вариант(простой).
poly *poly_get(const char *str)
{
    poly *result = malloc(sizeof(poly));
    poly *curr = result;
    const char *p = str;
    int sign = 1;
    int state = 1;
    do
    {
        if (!(*p))
        {
            if (state != 5)
            {
                printf("%s\n", "Error, expected state 5. Wrong input.");
                return NULL;
            }
            else break;
        }
        
        if (*p == ' ')
        {
            p++;
            continue;
        }
        if (state == 1)
        {
            char **end = &p;
            curr->coeff = strtol(p, end, 0) * sign;
            // if (!end)
            // {
            //     printf("end == NULL.");
            //     return NULL;
            // }
            p = *end;
            if (!curr->coeff)
            {
                printf("%s\n", "Wrong input. State 1, expected number.");
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
            }
            else
            {
                printf("%s\n", "Error state 2!");
                return NULL;
            }
            continue;
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
                printf("%s\n", "Error state 3!");
                return NULL;
            }
            continue;
        }
        if (state == 4)
        {
            curr->exp = strtol(p, &p, 0);
            if (!curr->exp)
            {
                printf("%s\n", "Wrong input. State 4, expected number.");
                return NULL;
            }
            ++state;
            continue;
        }
        if ((state == 5) && (*p == '-' || *p == '+'))
        {
            curr->next = malloc(sizeof(poly));
            curr = curr->next;
            sign = 44 - *p;
            state = 1;
            ++p;
            continue;
        }
        else
        {
            puts("Unknown error.");
        }
    }
    while (1);

    curr->next = NULL;
    
    return result;
}

poly *poly_add(poly *a, poly *b)
{
    if (!a || !b)
    {
        puts("Add error!");
        return NULL;
    }
    
    poly *result = malloc(sizeof(poly));
    poly *curr = result;
    poly *p1 = a;
    poly *p2 = b;
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
        printf("%dx^%d + ", curr->coeff, curr->exp);
        curr = curr->next;
    }
    printf("%dx^%d\n", curr->coeff, curr->exp);
}

void poly_free(poly *head)
{
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

int main()
{
    poly *head = malloc(sizeof(poly));
    poly *curr = head;
    for (size_t i = 0; i < 10; i++)
    {
        curr->coeff = 1;
        curr->exp = 1;
        curr->next = malloc(sizeof(poly));
        curr = curr->next;
    }
    curr->coeff = 1;
    curr->exp = 1;
    curr->next = NULL;

    poly_print(head);

     
    //char *p1 = "4x^2 + 5x^4 - 159x^8";
    char *p2 = "25x^1 - -14x^4 + 2x^100";
    poly *a = poly_get(p2);
    poly *b = poly_get("3x^2 - 1x^3 - 8x^4 + 158x^8 - 6x^13");
    poly_print(a);
    poly_print(b);
    poly *c = poly_add(a, b);
    poly_print(c);
    poly_free(c);

    return 0;
}