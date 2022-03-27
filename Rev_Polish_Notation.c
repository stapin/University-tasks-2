#include "stdio.h"
#include "stdlib.h"
#include "string.h"
//#include "setjmp.h"
#include "structlib.h"

#define OPER_COUNT 4
#define TOKENS_COUNT 19

#define NOT_FOUND (-1)
#define EMPTY_PTR (-2)
#define UNKNOWN_ERROR (-3)
#define INCORRECT_INPUT 0

#define TRUE 1
#define FALSE 0

#define STATE_1 1
#define STATE_2 2


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


//const char *operands[OPER_COUNT] = {"+", "-", "*", "/"};
static const int opPriority[OPER_COUNT] = {1, 1, 2, 2};
// Operands always must go ahead.
static const char *tokens[TOKENS_COUNT] = {
    "+", "-", "*", "/", "gcd", "lcm", "(", ")", ",",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
};
static const int tokens_len[] = {
    1, 1, 1, 1, 3, 3, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1
};

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
    COMMA,
    ZERO,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE
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

int is_digit(token_code code)
{
    if (code >= 9 && code <= 18) return TRUE;
    return FALSE;
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
            if (!is_digit(*code))
                (*ptr) += tokens_len[i];
            return 1;
        }
    }
    return 0;
}

int is_operand(token_code code)
{
    if (code <= 3) return TRUE;
    return FALSE;
}

int is_func(token_code code)
{
    if (code == GCD || code == LCM) return TRUE;
    return FALSE;
}

int tok_to_output(token_code code, char **out)
{
    if (!memcpy(*out, tokens[code], tokens_len[code]))
        return 0;
    *out += tokens_len[code];
    *((*out)++) = ' ';
    return 1;
}

size_t get_num_len(const char *str)
{
    char *end = NULL;
    long num = strtol(str, &end, 0);
    if (!num)
    {
        if (*str == '0')
            return 1;
        return 0;
    }
    return end - str;
}

void num_to_output(char **dst, const char **src, size_t len)
{
    memcpy(*dst, *src, len);
    *dst += len;
    *((*dst)++) = ' ';
    *src += len;
    return;
}

typedef struct error
{
    int code;
    char *ptr;
    char *message;
} error;

error *error_init()
{
    error *res = malloc(sizeof(error));
    res->code = 0;
    res->ptr = NULL;
    res->message = NULL;
    return res;
}

void error_set(error *err, int code, char *message, const char *ptr)
{
    err->code = code;
    --ptr;
    err->message = message;
    size_t len = strlen(ptr) < 50 ? strlen(ptr) : 50;
    err->ptr = malloc(len + 8);
    memcpy(err->ptr, ptr, len);
    err->ptr[len] = '\n';
    for (size_t i = 1; i < 6; i++)
        err->ptr[len + i] = ' ';
    
    err->ptr[len + 6] = '^';
    err->ptr[len + 7] = '\0';
    return;
}

void error_print(error *err)
{
    puts("____ ERRORS ____");
    printf("code: %d\nmessage: %s\nstr: %.60s\n", err->code, err->message, err->ptr);
    puts("________________");
    return;
}

int calc_RPN(const char *str, error *err)
{
    if (!*str || !str) return 0;
    stack *s = NULL;
    token_code curr_tok;
    int num;
    char *end = NULL;
    int tmp1, tmp2;
    while (*str)
    {
        while (*str == ' ')
        {
            ++str;
            continue;
        }
        num = strtol(str, &end, 0);
        if (str != end)
        {
            stack_push(&s, num);
            str = end;
        }
        else if (!read_token(&str, &curr_tok))
        {
            error_set(err, INCORRECT_INPUT, "Read token error.", str);
            return 0;
        }
        else if (is_operand(curr_tok) || is_func(curr_tok))
        {
            if (!s || !s->next)
            {
                error_set(err, INCORRECT_INPUT, "Wrong input. Less args for function or operator.", str);
                return 0;
            }
            tmp1 = stack_pop(&s);
            tmp2 = stack_pop(&s);
            if (curr_tok == ADD) stack_push(&s, tmp2 + tmp1);
            else if (curr_tok == SUB) stack_push(&s, tmp2 - tmp1);
            else if (curr_tok == MUL) stack_push(&s, tmp2 * tmp1);
            else if (curr_tok == DIV) stack_push(&s, tmp2 / tmp1);
            else if (curr_tok == GCD) stack_push(&s, gcd(tmp1, tmp2));
            else if (curr_tok == LCM) stack_push(&s, lcm(tmp1, tmp2));
        }
        else 
        {
            error_set(err, UNKNOWN_ERROR, "Unknown error.", str);
            return 0;
        }
    }
    int result = s->value;
    stack_free(s);
    
    return result;
}


char *inftoRPN(const char *infix, error *err)
{
    if (!err)
    {
        puts("Err is NULL");
        return NULL;
    }    
    if (!infix)
    {
        error_set(err, EMPTY_PTR, "Error, arg inftoRPN is NULL.", NULL);
        return NULL;
    }
    char *result = malloc(strlen(infix) * 2 + 1);
    char *resptr = result;
    stack *stok = NULL;
    token_code curTok;
    int state = 2;
    int brk_count = 0;
    
    size_t num_len = 0;

    while (*infix)
    {
        if (*infix == ' ')
        {
            ++infix;
            continue;
        }
        if (!read_token((const char **)(&infix), &curTok))
        {
            error_set(err, INCORRECT_INPUT, "Error, expected token.", infix);
            return NULL;
        }
        if (state == 1)
        {
            if (is_operand(curTok))
            {
                state = 2;
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
            if (curTok == CL_BRK)
            {
                state = 1;
                --brk_count;
                if (brk_count < 0)
                {
                    error_set(err, INCORRECT_INPUT, "Too much cl_brk.", infix);
                    return NULL;
                }
                int lastTok = stack_pop(&stok);
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
                    continue;
                }
                stack_push(&stok, lastTok);
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
                token_code tmp = stack_pop(&stok);
                if (!is_func(tmp))
                {
                    error_set(err, INCORRECT_INPUT, "Comma used incorrect.", infix);
                    return NULL;
                }
                stack_push(&stok, tmp);
                stack_push(&stok, lastTok);
                state = 2;
                continue;
            }
        }
        if (state == 2)
        {
            if (curTok == OP_BRK)
            {
                ++brk_count;
                stack_push(&stok, OP_BRK);
                continue;
            }
            if (is_func(curTok))
            {
                state = 3;
                stack_push(&stok, curTok);
                continue;
            }
            if (curTok == ADD || curTok == SUB)
            {
                --infix;
                num_len = get_num_len(infix);
                if (num_len)
                {
                    num_to_output(&resptr, &infix, num_len);
                    state = 1;
                    continue;
                }
                else
                {
                    error_set(err, INCORRECT_INPUT, "Incorrect input. Expected unary + or -", infix);
                    return NULL;
                }
            }
            if (is_digit(curTok))
            {
                num_len = get_num_len(infix);
                if (num_len)
                {
                    num_to_output(&resptr, &infix, num_len);
                    state = 1;
                    continue;
                }
                else
                {
                    error_set(err, INCORRECT_INPUT, "Unknown error. State 2", infix);
                    return NULL;
                }
            }
            else
            {
                error_set(err, INCORRECT_INPUT, "Wrong input. Expected op_brk || func || number", infix);
                return NULL;
            }
        }
        if (state == 3)
        {
            if (curTok == OP_BRK)
            {
                ++brk_count;
                stack_push(&stok, OP_BRK);
                state = 2;
                continue;
            }
            else
            {
                error_set(err, INCORRECT_INPUT, "Wrong input. Expected op_brk after function.", infix);
                return NULL;
            }
        }
        else
        {
            error_set(err, INCORRECT_INPUT, "Unknown error.", infix);
            return NULL;
        }

    }
    if (brk_count != 0)
    {
        error_set(err, INCORRECT_INPUT, "Less cl_brk.", infix);
        return NULL;
    }
    while (stok)
    {
        token_code code = stack_pop(&stok);
        tok_to_output(code, &resptr);
    }
    resptr--;
    *resptr = '\0';
    result = realloc(result, resptr - result + 1);
    return result;
}



// void test()
// {
//     char s[] = "1 3 + 2 4 - * 18 + 15 gcd 21 lcm";
//     int res = calc_RPN(s);
//     printf("%d", res);
// }

void test2()
{
    //char s[] = "2 * (3 + 4) - (6 + 4) / 2";
    char s[] = "lcm(8 * 3, 4 * 3)()) - (-1) + 2 - 2*(gcd(10, 3*5) + lcm(5, (3-2)))";
    //char s[] = "1+2*gcd(3,2+4)*(5-1)";
    
    error *err = error_init();

    char *res = inftoRPN(s, err);
    error_print(err);
    if (!res)
    {
        return;
    }
    puts(res);
    int res2 = calc_RPN(res, err);
    printf("result: %d", res2);
}

int main()
{
    //test();
    test2();
    return 0;
}