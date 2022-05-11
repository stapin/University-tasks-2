#include "error.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

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
    err->message = message;
    if (ptr == NULL)
    {
        err->ptr = NULL;
        return;
    }
    --ptr;
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
