#pragma once

#include <stdlib.h>
#include <stdbool.h>

// Structure to hold a list of tokens
typedef struct {
    char ** items;
    size_t size;
} tokenlist;

// Function declarations for token management
char * get_input(void);
tokenlist * get_tokens(char *input);
tokenlist * new_tokenlist(void);
void add_token(tokenlist *tokens, char *item);
void free_tokens(tokenlist *tokens);
