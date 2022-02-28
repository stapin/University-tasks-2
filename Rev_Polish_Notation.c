#include "stdio.h"
#include "stdlib.h"


typedef struct stack
{
    int value;
    struct stack *next;
} stack;

stack *stack_init(int value)
{
    stack *result = malloc(sizeof(stack));
    result->value = value;
    result->next = NULL;
    return result;
}

stack *stack_push(stack **head, int value)
{
    if (!(*head)) 
    {
        *head = stack_init(value);
        return NULL;
    }
    stack *curr = malloc(sizeof(stack));
    curr->value = value;
    curr->next = *head;
    *head = curr;
    return NULL;
}


void stack_free(stack *s)
{
    stack *curr = s;
    while (curr->next)
    {
        s = curr;
        curr = curr->next;
        free(s);
    }
}

int stack_pop(stack **head)
{
    if ((*head)->next == NULL)
    {
        int res = (*head)->value;
        stack *tmp = *head;
        stack_free(tmp);
        *head = NULL;
        return res;
    }
    stack *tmp = *head;
    int res = (*head)->value;
    *head = (*head)->next;
    free(tmp);
    return res;
}


int gcd(int a, int b)
{
    while(a && b)
    {
        if (a > b) a %= b;
        else b %= a;
    }
    if (a) return a;
    else return b;
}

int lcm(int a, int b)
{
    return a * b / gcd(a, b);
}

int calc_RPN(const char *str)
{
    if (!*str || !str) return 0;
    stack *s = NULL;
    const char *p = str;
    char *end = NULL;
    int num;
    int tmp1, tmp2;
    while (*p)
    {
        while (*p == ' ') ++p;
        if (*p == '+' || *p == '-' || *p == '*' || *p == '/')
        {
            if (!s || !s->next)
            {
                puts("Wrong input. Stack overflow.");
                return 0;
            }
            tmp1 = stack_pop(&s);
            tmp2 = stack_pop(&s);
            if (*p == '+') stack_push(&s, tmp2 + tmp1);
            else if (*p == '-') stack_push(&s, tmp2 - tmp1);
            else if (*p == '*') stack_push(&s, tmp2 * tmp1);
            else if (*p == '/') stack_push(&s, tmp2 / tmp1);

            ++p;
            continue;
        }

        if (*p == 'g')
        {
            ++p;
            if (*(p++) == 'c')
            {
                if (*(p++) == 'd')
                {
                    if (!s || !s->next)
                    {
                        puts("Wrong input. Less args fo lcm of gcd, expected 2.");
                        return 0;
                    }
                    // s->next->value = gcd(s->next->value, s->value);
                    // stack_pop(&s);
                    tmp1 = stack_pop(&s);
                    tmp2 = stack_pop(&s);
                    stack_push(&s, gcd(tmp1, tmp2));
                    continue;
                }
                puts("Wrong name for gcd.");
                return 0;    
            }
            puts("Wrong name for gcd.");
            return 0;
        }

        if (*p == 'l')
        {
            ++p;
            if (*(p++) == 'c')
            {
                if (*(p++) == 'm')
                {
                    if (!s || !s->next)
                    {
                        puts("Wrong input. Less args fo lcm of gcd, expected 2.");
                        return 0;
                    }
                    s->next->value = lcm(s->next->value, s->value);
                    stack_pop(&s);
                    continue;
                }
                puts("Wrong name for lcm.");
                return 0;
            }
            puts("Wrong name for lcm.");
            return 0;
        }

        num = strtol(p, &end, 0);
        if (!num)
        {
            puts("Wrong input, number expected.");
            return 0;
        }
        stack_push(&s, num);
        p = end;
    }
    int result = s->value;
    stack_free(s);
    return result;
}

// 3 + 5 + 7 - 9 -> 3 5 + 7 + 9 -
char *inftoRPN(const char *str)
{
    if (!str)
    {
        puts("Error, arg inftoRPN is NULL.");
        return NULL;
    }
    stack *oper = NULL;
    stack *s = NULL;
    size_t resLen = 0;
    int state = 1;
    int *end = NULL;
    int numLen = 0;
    int num = 0;
    // state | expectation
    // 1     | int || ( || gcd || lcm
    // 2     | + || - || / || * || ( || )
    while (str)
    {
        if (state == 1)
        {
            num = strtol(str, &end, 0);
            if (!num && *str != '0')
            {
                state = 2;
                continue;
            }
            resLen += end - str;
            str = end;
            stack_push(&s, num);
        }
        if (state == 2)
        {

        }

    }
    if (state != 1)
    {
        puts("Wrong input!");
        return NULL;
    }
    
}

void test()
{
    char s[] = "1 3 + 2 4 - * 18 + 15 gcd 25 lcm";
    int res = calc_RPN(s);
    printf("%d", res);
}

int main()
{
    //char s[] = "1+2*gcd(3,2+4)*(5-1)";
    test();
    return 0;
}