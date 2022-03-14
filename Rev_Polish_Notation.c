#include "stdio.h"
#include "stdlib.h"
#include "string.h"
//#include "setjmp.h"
#define OPER_COUNT 4
#define TOKENS_COUNT 9

#define NOT_FOUND (-1)
#define EMPTY_PTR (-2)
#define INCORRECT_INPUT 0

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
    if (!(*head)) return 0;
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
    if (a < 0) a = -a;
    if (b < 0) b = -b;
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
    if (!(a && b)) return 0;
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
                        puts("Wrong input. Less args for lcm of gcd, expected 2.");
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
                        puts("Wrong input. Less args for lcm of gcd, expected 2.");
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



// Можно цифры тоже сделать в виде токенов.
char const *operands[OPER_COUNT] = {"+", "-", "*", "/"};
int opPriority[OPER_COUNT] = {1, 1, 2, 2};
char const *tokens[TOKENS_COUNT] = {"+", "-", "*", "/", "gcd", "lcm", "(", ")", ","};
int tokens_len[] = {1, 1, 1, 1, 3, 3, 1, 1, 1};
char const *funcs[] = {"gcd", "lcm"};
typedef enum token_code
{
    ADD,
    SUB,
    MUL,
    DIV,
    GCD,
    LCM,
    OP_BRK,
    CL_BRK,
    COMMA
} token_code;

int strcmpr(const char *str1, const char *str2, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (str1[i] == str2[i]) continue;
        return 0;
    }
    return 1;
}

int read_token(const char **ptr, token_code *code)
{
    while (**ptr == ' ') ++(*ptr);
    if (!ptr) return 0;
    if (!(*ptr)) return 0;
    for (size_t i = 0; i < TOKENS_COUNT; i++)
    {
        if (strcmpr(*ptr, tokens[i], tokens_len[i]))
        {
            *code = i;
            (*ptr) += tokens_len[i];
            return 1;
        }
    }
    return 0;
}

int is_operand(token_code code)
{
    if (code <= 3) return 1;
    return 0;
}

int is_func(token_code code)
{
    if (code == GCD || code == LCM) return 1;
    return 0;
}

int tok_to_output(token_code code, char **out)
{
    if (!memcpy(*out, tokens[code], tokens_len[code]))
        return 0;
    *out += tokens_len[code];
    *((*out)++) = ' ';
    return 1;
}

char *inftoRPN(const char *infix)
{
    if (!infix)
    {
        puts("Error, arg inftoRPN is NULL.");
        return NULL;
    }

    char *result = malloc(strlen(infix) + 1);
    char *resptr = result;
    stack *stok = NULL;
    token_code curTok;
    int state = 0;
    int num = 0;
    char *end = NULL;
    //int opBrackCount = 0;
    while (*infix)
    {
        if (*infix == ' ')
        {
            ++infix;
            continue;
        }
        if (state == 0)
        {
            num = strtol(infix, &end, 0);
            if (!num && (*infix) != '0')
            {
                state = 1;
                continue;
            }
            memcpy(resptr, infix, end - infix);
            resptr += end - infix;
            infix = end;
            *(resptr++) = ' ';
            state = 1;
            continue;
        }
        if (state == 1)
        {
            if (!read_token((const char **)(&infix), &curTok))
            {
                puts("Error, expected token.");
                return NULL;
            }
            if (is_operand(curTok))
            {
                state = 0;
                if (!stok) 
                {
                    stack_push(&stok, curTok);
                    continue;
                }
                token_code lastTok = stack_pop(&stok);
                if (is_operand(lastTok))
                {
                    if (opPriority[curTok] <= opPriority[lastTok])
                    {
                        tok_to_output(lastTok, &resptr);
                        stack_push(&stok, curTok);
                        continue;
                    }
                }
                stack_push(&stok, lastTok);
                stack_push(&stok, curTok);
                continue;
            }
            if (curTok == OP_BRK)
            {
                stack_push(&stok, OP_BRK);
                state = 0;
                continue;
            }
            if (curTok == CL_BRK)
            {
                token_code lastTok = stack_pop(&stok);
                while (lastTok != OP_BRK)
                {
                    tok_to_output(lastTok, &resptr);
                    lastTok = stack_pop(&stok);
                }
                if (!stok) continue;
                lastTok = stack_pop(&stok);
                if (is_func(lastTok))
                {
                    tok_to_output(lastTok, &resptr);
                    state = 0;
                    continue;
                }
                stack_push(&stok, lastTok);
                state = 1;
                continue;
            }
            if (is_func(curTok))
            {
                stack_push(&stok, curTok);
                continue;
            }
            if (curTok == COMMA)
            {

                token_code lastTok = stack_pop(&stok);
                while (lastTok != OP_BRK)
                {
                    tok_to_output(lastTok, &resptr);
                    lastTok = stack_pop(&stok);
                }
                stack_push(&stok, lastTok);
                state = 0;
                continue;
            }
            else
            {
                puts("Unknown error.");
                return NULL;
            }
        }
    }
    while (stok)
    {
        token_code code = stack_pop(&stok);
        tok_to_output(code, &resptr);
    }
    resptr--;
    *resptr = '\0';
    //result = realloc(result, resptr - result + 1);
    return result;
}

void test()
{
    char s[] = "1 3 + 2 4 - * 18 + 15 gcd 25 lcm";
    int res = calc_RPN(s);
    printf("%d", res);
}

void test2()
{
    //char s[] = "2 * (3 + 4) - (6 + 4) / 2";
    char s[] = "lcm(8 * 3, 4 * 3) + 2 - 2*(gcd(10, 3*5) + lcm(5, (3-2)))";
    //char s[] = "1+2*gcd(3,2+4)*(5-1)";
    
    char *res = inftoRPN(s);
    if (!res)
    {
        return;
    }
    puts(res);
    int res2 = calc_RPN(res);
    printf("%d", res2);
}

int main()
{
    //char s[] = "1+2*gcd(3,2+4)*(5-1)";
    //test();
    test2();
    return 0;
}