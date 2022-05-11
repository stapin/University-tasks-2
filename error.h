#pragma once
#ifndef ERROR_H
#define ERROR_H


typedef struct error
{
    int code;
    char *ptr;
    char *message;
} error;


error *error_init();

void error_set(error *err, int code, char *message, const char *ptr);

void error_print(error *err);


#endif