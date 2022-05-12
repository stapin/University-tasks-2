#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "error.h"
#include "dfa.h"

#define TOKENS_COUNT 8

#define NOT_FOUND (-1)
#define EMPTY_PTR (-2)
#define UNKNOWN_ERROR (-3)
#define INCORRECT_INPUT (-4)

#define TRUE 1
#define FALSE 0

#define STATE_1 1
#define STATE_2 2
#define STATE_3 3

#define UNARY 0
#define BIN_1 1
#define BIN_2 2


typedef struct node_t
{
    void *data;
    struct node_t *next;
} node_t;

typedef struct stack_t
{
    node_t *head;
} stack_t;  

stack_t *stack_init()
{
    stack_t *result = malloc(sizeof(stack_t));
    result->head = NULL;
    return result;
}

int stack_is_empty(stack_t *s)
{
    if (s == NULL) return 1;
    if (s->head == NULL) return 1;
    return 0;
}

int stack_push(stack_t *s, void *data)
{
    if (s == NULL) return 0;
    node_t *tmp = s->head;
    s->head = malloc(sizeof(node_t));
    s->head->data = data;
    s->head->next = tmp;
    return 1;
}

void *stack_pop(stack_t *s)
{
    if (s == NULL) return NULL;
    if (s->head == NULL) return NULL;
    void *res = s->head->data;
    node_t *tmp = s->head->next;
    free(s->head);
    s->head = tmp;
    return res;
}

typedef enum token_type
{
    OPERATOR,
    CL_BRK,
    OP_BRK,
    IDENTIFIER,
    NUMBER
} token_type;

typedef struct token
{
    char *str;
    int len;
    token_type type;
    int priority;
} token;

token *token_init(char *str, int len, token_type type, int prior)
{
    token *res = malloc(sizeof(token));
    res->str = str;
    res->len = len;
    res->type = type;
    res->priority = prior;
    return res;
}

token *token_copy(token tok)
{
    token *res = malloc(sizeof(token));
    res->str = tok.str;
    res->len = tok.len;
    res->type = tok.type;
    res->priority = tok.priority;
    return res;
}



static const token tokens[TOKENS_COUNT] = {
    {"!", 1, OPERATOR, 0}, {"+", 1, OPERATOR, 1}, {"^", 1, OPERATOR, 2}, {"-", 1, OPERATOR, 1},
    {"(", 1, OP_BRK, 0}, {")", 1, CL_BRK, 0}, {"l1", 2, IDENTIFIER, 0}, {"l2", 2, IDENTIFIER, 0}
};



int tokencmpr(token tok, const char *str, int *len)
{
    char *ptr = tok.str;
    *len = 0;
    while (*ptr)
    {
        if (*ptr != *str) 
        {
            *len = 0;
            return 0;
        }
        ++ptr;
        ++str; 
        ++(*len);   
    }
    return 1;
}

int is_digit(char c)
{
    if (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || 
        c == '5' || c == '6' || c == '7' || c == '8' || c == '9' )
        return 1;
    return 0;
}

int is_letter(const char c)
{
    if ( (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') )
        return 1;
    return 0;
}

int read_identifier(const char **ptr, token **result)
{
    if ( !is_letter(**ptr) )
        return 0;
    const char *start = *ptr;
    ++(*ptr);
    while (is_letter(**ptr) || is_digit(**ptr) || **ptr == '_')
    {
        ++(*ptr);
    }
    int len = *ptr - start;
    token res = {NULL, len, IDENTIFIER, 0};
    res.str = malloc(len + 1);
    res.str = memcpy(res.str, start, len);
    res.str[len] = '\0';
    *result = token_copy(res);
    return 1;
}

int read_token(const char **ptr, token **result)
{
    while (**ptr == ' ') ++(*ptr);
    if (!ptr) return 0;
    if (!(*ptr)) return 0;
    int len;

    /* trying to read token */
    for (size_t i = 0; i < TOKENS_COUNT; i++)
    {
        if (tokencmpr(tokens[i], *ptr, &len))
        {
            *result = token_copy(tokens[i]);
            (*ptr) += len;
            return 1;
        }
    }
    
    /* trying to read number */
    if (is_digit(**ptr))
    {
        *result = token_init(NULL, 0, NUMBER, 0);
        return 1;
    }
    
    /* trying to read identifier */
    if (read_identifier(ptr, result)) return 1;


    return 0;
}


int tok_to_output(token *tok, char **out)
{
    if (!memcpy(*out, tok->str, tok->len))
        return 0;
    *out += tok->len ;
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

void unary_op_check(stack_t *s, char **out)
{
    if (stack_is_empty(s)) return;
    token *last_tok = stack_pop(s);
    if (last_tok->type == OPERATOR && last_tok->priority == 0)
    {
        tok_to_output(last_tok, out);
    }
    else
    {
        stack_push(s, last_tok);
    }
}

char *dfa_inf_to_rpn(const char *infix, error *err)
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
    stack_t *stok = stack_init();
    token *curr_tok = NULL;
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
        if (!read_token((const char **)(&infix), &curr_tok))
        {
            error_set(err, INCORRECT_INPUT, "Error, expected token.", infix);
            return NULL;
        }

        switch (state)
        {
        case STATE_1:
            
            if (curr_tok->type == OPERATOR)
            {
                state = 2;
                if (stack_is_empty(stok)) 
                {
                    stack_push(stok, curr_tok);
                    break;
                }
                if (curr_tok->priority == 0)
                {
                    error_set(err, INCORRECT_INPUT, "Expected binary operator, but unary was given.", infix);
                    return NULL;
                }
                token *last_tok = stack_pop(stok);
                if (last_tok->type == OPERATOR)
                {
                    if (curr_tok->priority <= last_tok->priority)
                    {
                        tok_to_output(last_tok, &resptr);
                        stack_push(stok, curr_tok);
                        break;
                    }
                }
                stack_push(stok, last_tok);
                stack_push(stok, curr_tok);
            }

            if (curr_tok->type == CL_BRK)
            {
                state = 1;
                --brk_count;
                if (brk_count < 0)
                {
                    error_set(err, INCORRECT_INPUT, "Too much cl_brk.", infix);
                    return NULL;
                }
                token *last_tok = stack_pop(stok);
                while (last_tok->type != OP_BRK)
                {
                    tok_to_output(last_tok, &resptr);
                    last_tok = stack_pop(stok);
                }

                unary_op_check(stok, &resptr);
            }

            break;
        
        case STATE_2:

            if (curr_tok->type == OP_BRK)
            {
                ++brk_count;
                stack_push(stok, curr_tok);
                break;
            }
            if (curr_tok->type == IDENTIFIER)
            {
                tok_to_output(curr_tok, &resptr);
                state = 1;
                
                unary_op_check(stok, &resptr);
                break;
            }
            if (curr_tok->type == NUMBER)
            {
                num_len = get_num_len(infix);
                if (num_len)
                {
                    num_to_output(&resptr, &infix, num_len);
                    state = 1;
                    unary_op_check(stok, &resptr);
                    break;
                }
                else
                {
                    error_set(err, INCORRECT_INPUT, "Unknown error. State 2", infix);
                    return NULL;
                }
            }
            if (curr_tok->type == OPERATOR)
            {
                if (curr_tok->priority != 0)
                {
                    error_set(err, INCORRECT_INPUT, "Expected unary operator, bur binary was given.", infix);
                    return NULL;
                }
                stack_push(stok, curr_tok);
                break;
            }
            else
            {
                error_set(err, INCORRECT_INPUT, "Wrong input. Expected op_brk || number", infix);
                return NULL;
            }


        case STATE_3:

            if (curr_tok->type == OP_BRK)
            {
                ++brk_count;
                stack_push(stok, curr_tok);
                state = 2;
                break;
            }
            else
            {
                error_set(err, INCORRECT_INPUT, "Wrong input. Expected op_brk after function.", infix);
                return NULL;
            }

        default:
            error_set(err, INCORRECT_INPUT, "Unknown error.", infix);
            return NULL;
        }
    }

    if (brk_count != 0)
    {
        error_set(err, INCORRECT_INPUT, "Less cl_brk.", infix);
        return NULL;
    }
    while (!stack_is_empty(stok))
    {
        token *tok = stack_pop(stok);
        tok_to_output(tok, &resptr);
    }
    resptr--;
    *resptr = '\0';
    result = realloc(result, resptr - result + 1);
    return result;
}

typedef struct dfa_id 
{
    char *str;
    dfa *a;
} dfa_id;

void dfa_id_print(dfa_id **id_list, int len)
{
    for (int i = 0; i < len; i++)
    {
        dfa_id *curr = id_list[i];
        printf("%s, ", curr->str);
    }
    printf("\n");
}

int str_is_equal(const char *str1, const char *str2)
{
    while (*str1 || *str2)
    {
        if (*str1 != *str2) return 0;
        ++str1;
        ++str2;
    }
    return 1;
}

int is_in_list(token *tok, dfa_id **list, int list_len, int *ind)
{
    for (int i = 0; i < list_len; i++)
    {
        dfa_id *curr_tok = list[i];
        if (str_is_equal(tok->str, curr_tok->str))
        {
            *ind = i;
            return 1;
        }
    }
    return 0;
}

dfa *dfa_calc_rpn(const char *str, dfa_id **list, int list_len, error *err)
{
    stack_t *s = stack_init();
    token *curr_tok = NULL;
    while (*str)
    {
        if (!read_token(&str, &curr_tok))
        {
            error_set(err, INCORRECT_INPUT, "Expected token.", str);
            return NULL;
        }
        if (curr_tok->type == IDENTIFIER)
        {
            int ind;
            if (is_in_list(curr_tok, list, list_len, &ind))
            {
                stack_push(s, list[ind]->a);
            }
            else
            {
                error_set(err, INCORRECT_INPUT, "No such identifier.", str);
                return NULL;
            }
        }
        else if (curr_tok->type == NUMBER)
        {
            char *end = NULL;
            int num = strtol(str, &end, 0);
            str = end;
            dfa *a = int_dfa(num);
            stack_push(s, a);
        }
        else if (curr_tok->type == OPERATOR)
        {
            dfa *a1;
            if (curr_tok->priority == UNARY)
            {
                if (stack_is_empty(s))
                {
                    error_set(err, INCORRECT_INPUT, "Incorrect input for unary operation.", str);
                    return NULL;
                }
                a1 = stack_pop(s);
                stack_push(s, dfa_complement(a1));
                continue;
            }

            a1 = stack_pop(s);
            dfa *a2 = stack_pop(s);

            if (a1 == NULL || a2 == NULL)
            {
                error_set(err, INCORRECT_INPUT, "Incorrect input for binary operation.", str);
                return NULL;
            }
            
            if (*curr_tok->str == '+') stack_push(s, dfa_union(a1, a2));
            else if (*curr_tok->str == '-') stack_push(s, dfa_difference(a2, a1));
            else if (*curr_tok->str == '^') stack_push(s, dfa_intersection(a1, a2));
        }
        else
        {
            error_set(err, INCORRECT_INPUT, "Unknown error. [calc_rpn]", str);
            return NULL;
        }
    }
    dfa *res = stack_pop(s);
    free(s);
    return res;
}


dfa *dfa_calc_str(const char *str, dfa_id **id_list, int list_len, error *err)
{
    char *rpn = dfa_inf_to_rpn(str, err);
    dfa *result = dfa_calc_rpn(rpn, id_list, list_len, err);
    return result;
}

#define FUNCTIONS_COUNT 3

#define INIT_CODE 0
#define EXIT_CODE 1
#define PRINT_CODE 2
#define SUCCEED_CODE 3
#define ERROR_CODE (-1)

const token functions[] = {
    {"exit()", 6, OPERATOR, 1}, {"print(", 6, OPERATOR, 2}, {"=", 1, NUMBER, 3}
};

int read_token2(const char **ptr, token **result)
{
    while (**ptr == ' ') ++(*ptr);
    if (!ptr) return 0;
    if (!(*ptr)) return 0;
    int len;

    /* trying to read function */
    for (size_t i = 0; i < FUNCTIONS_COUNT; i++)
    {
        if (tokencmpr(functions[i], *ptr, &len))
        {
            *result = token_copy(functions[i]);
            (*ptr) += len;
            return 1;
        }
    }
    
    /* trying to read identifier */
    if (read_identifier(ptr, result)) return 1;


    return 0;
}

dfa_id *dfa_parse_string(const char *str, int *code, error *err, dfa_id **id_list, int list_len)
{
    token *tok = NULL;
    if (!read_token2(&str, &tok))
    {
        error_set(err, INCORRECT_INPUT, "Read token error. [parse_string]", str);
        *code = ERROR_CODE;
        return NULL;
    }
    if (tok->type == OPERATOR)
    {
        if (tok->priority == 1)
        {
            *code = EXIT_CODE;
            return NULL;
        }
        if (tok->priority == 2)
        {
            /* Чтобы захватить скобку */
            --str;

            dfa *result = NULL;
            result = dfa_calc_str(str, id_list, list_len, err);
            if (result == NULL)
            {
                *code = ERROR_CODE;
                return NULL;
            }
            *code = PRINT_CODE;
            dfa_print(result, 50);
            return NULL;
        }
        else
        {
            error_set(err, UNKNOWN_ERROR, "Token error. Expected function.", NULL);
            *code = ERROR_CODE;
            return NULL;
        }
    }
    else if (tok->type == IDENTIFIER)
    {
        token *tok2 = NULL;
        if (!read_token2(&str, &tok2))
        {
            error_set(err, INCORRECT_INPUT, "Read token error. [parse_string | IDENTIFIER]", str);
            *code = ERROR_CODE;
            return NULL;
        }
        if (tok2->type != NUMBER || tok2->priority != 3)
        {
            error_set(err, INCORRECT_INPUT, "Token error. Expected token '='", str);
            *code = ERROR_CODE;
            return NULL;
        }

        dfa *a = dfa_calc_str(str, id_list, list_len, err);
        if (a == NULL)
        {
            *code = ERROR_CODE;
            return NULL;
        }
        dfa_id *result = malloc(sizeof(dfa_id));
        result->a = a;
        result->str = malloc(sizeof(tok->len + 1));
        result->str = memcpy(result->str, tok->str, tok->len + 1);
        *code = SUCCEED_CODE;
        return result;
    }
    else
    {
        error_set(err, UNKNOWN_ERROR, "Unknown error. [parse_string]", NULL);
        *code = ERROR_CODE;
        return NULL;
    }
}

dfa_id **id_list_init()
{
    dfa_id **id_list = malloc(sizeof(dfa_id*) * 10);
    id_list[0] = malloc(sizeof(id_list));
    id_list[0]->a = L2_init();
    id_list[0]->str = "l2";
    id_list[1] = malloc(sizeof(id_list));
    id_list[1]->a = L1_init();
    id_list[1]->str = "l1";
    return id_list;
}

void dfa_interpreter(error *err)
{
    dfa_id **id_list = id_list_init();
    int list_capacity = 10;
    int id_num = 2;
    int code = INIT_CODE;
    while (1)
    {
        char str[100];
        if (fgets(str, 100, stdin) == NULL)
        {
            error_set(err, INCORRECT_INPUT, "Scan error.", NULL);
            error_print(err);
            return;
        }
        str[strcspn(str, "\n")] = 0;
        dfa_id *new_id = dfa_parse_string(str, &code, err, id_list, id_num);
        //dfa_id_print(id_list, id_num);
        if (new_id == NULL)
        {
            if (code == ERROR_CODE)
            {
                error_print(err);
            }
            else if (code == EXIT_CODE)
            {
                break;
            }
        }
        else
        {

            if (list_capacity <= id_num + 1)
            {
                list_capacity *= 2;
                id_list = (dfa_id**)realloc(id_list, list_capacity * sizeof(dfa_id*));
                if (id_list == NULL)
                {
                    error_set(err, UNKNOWN_ERROR, "Realloc error. [dfa_interpreter]", NULL);
                    error_print(err);
                    return;
                }
            }
            id_list[id_num] = new_id;
            ++id_num;
        }

    }
    
}

void test()
{
    error *err = error_init();
    char *str = "(b + (4 - a1 ^ a2)) ^ 3 + !(f1 + f2) ^ f2";
    char *res = dfa_inf_to_rpn(str, err);
    if (res == NULL)
    {
        error_print(err);
        return;
    }
    

    printf("%s\n", res);
}

dfa_id **dfa_list_init()
{
    int len = 4;
    dfa_id **dfa_list = malloc(sizeof(dfa_id*) * len);
    for (int i = 0; i < len; i++)
    {
        dfa_list[i] = malloc(sizeof(dfa));
    }
    
    dfa_list[0]->a = int_dfa(5);
    dfa_list[0]->str = "i5";
    dfa_list[1]->a = int_dfa(7);
    dfa_list[1]->str = "i7";
    dfa_list[2]->a = L2_init();
    dfa_list[2]->str = "l2";
    dfa_list[3]->a = L1_init();
    dfa_list[3]->str = "l1";

    return dfa_list;
}

void test2()
{
    error *err = error_init();
    char *str = "i5 + (i7 + l2 ^ (3 + 15)) - l2 - 5 + l1";
    char *res1 = dfa_inf_to_rpn(str, err);
    if (res1 == NULL)
    {
        error_print(err);
        return;
    }
    printf("input: %s\n", str);
    printf("rpn: %s\n", res1);

    dfa_id **dfa_list = dfa_list_init();
    dfa *res2 = dfa_calc_rpn(res1, dfa_list, 4, err);
    if (res2 == NULL)
    {
        error_print(err);
        return;
    }
    printf("%s", "output: ");
    dfa_print(res2, 20);

}


void interpreter_test()
{

    error *err = error_init();
    dfa_interpreter(err);
}

int main()
{
    //test2();
    interpreter_test();
    return 0;
}