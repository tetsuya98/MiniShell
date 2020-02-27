#include <time.h>

#ifndef PARSER_H
#define PARSER_H

#define MAX_STR_SIZE 2048
#define MAX_TOKENS 1025

typedef enum {
    SEMICOL,
    AND,
    OR,
    PARAL,
    PIPE,
    END,
    EGAL
} separator_t;

typedef enum {
    INPUT,
    OUTPUT,
    OUTPUT_APPEND,
    ERROR,
    ERROR_APPEND
} redirection_t;

int trim(char* str);
int clean(char* str);
size_t split_str(char* str, char* tab[], size_t max_size);
int command(char* tab[], separator_t* sep);
int redirection(char* tab[], redirection_t* redir);

#endif
