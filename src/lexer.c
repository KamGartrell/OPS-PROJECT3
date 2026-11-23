#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to read a line of input from the user, handling lines longer than 4 characters
char *getInput(void) {
	char *buffer = NULL;
	int bufsize = 0;
	char line[5];

	// Read input in chunks of 4 characters until a newline is encountered
	while (fgets(line, 5, stdin) != NULL) {
		int addBy = 0;
		char *newln = strchr(line, '\n');
		if (newln != NULL)
			addBy = newln - line;
		else
			addBy = 5 - 1;
		buffer = (char *)realloc(buffer, bufsize + addBy);
		memcpy(&buffer[bufsize], line, addBy);
		bufsize += addBy;
		if (newln != NULL)
			break;
	}
	buffer = (char *)realloc(buffer, bufsize + 1);
	buffer[bufsize] = 0;
	return buffer;
}

// Function to create a new tokenlist structure
tokenlist *newTokenlist(void) {
	tokenlist *tokens = (tokenlist *)malloc(sizeof(tokenlist));
	tokens->size = 0;
	tokens->items = (char **)malloc(sizeof(char *));
	tokens->items[0] = NULL; /* make NULL terminated */
	return tokens;
}

// Function to add a token to the tokenlist
void addToken(tokenlist *tokens, char *item) {
	int i = tokens->size;

	tokens->items = (char **)realloc(tokens->items, (i + 2) * sizeof(char *));
	tokens->items[i] = (char *)malloc(strlen(item) + 1);
	tokens->items[i + 1] = NULL;
	strcpy(tokens->items[i], item);

	tokens->size += 1;
}

// Function to tokenize the input string into a tokenlist
tokenlist *getTokens(char *input) {
	char *buf = (char *)malloc(strlen(input) + 1);
	strcpy(buf, input);
	tokenlist *tokens = newTokenlist();
	char *tok = strtok(buf, " ");
	while (tok != NULL)
	{
		addToken(tokens, tok);
		tok = strtok(NULL, " ");
	}
	free(buf);
	return tokens;
}

// Function to free the memory allocated for a tokenlist
void freeTokens(tokenlist *tokens) {
	for (int i = 0; i < tokens->size; i++)
		free(tokens->items[i]);
	free(tokens->items);
	free(tokens);
}
