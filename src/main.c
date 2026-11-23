#include "fat32.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Main function that initializes the FAT32 file system and handles user commands
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("invalid number of arguments.\n");
        return -1;
    }

    if (mountFat32(argv[1]) != 0) {
        printf("%s does not exist.\n", argv[1]);
        return -1;
    }
    
    // Main command loop to process user input
    while (1) {
        if (cwdPath[0] == '\0') {
            printf("%s/> ", imgName);
        } else {
            printf("%s/%s/> ", imgName, cwdPath);
        }

        char *input = getInput();
        tokenlist *tokens = getTokens(input);

        // Process the command based on the first token
        if (tokens->size > 0) {
            char *cmd = tokens->items[0];
            // Handle the "exit" command to terminate the program
            if (strcmp(cmd, "exit") == 0) {
                free(input);
                freeTokens(tokens);
                break;
            } else if (strcmp(cmd, "info") == 0) {
                wrapper(cmdInfo, tokens);
            } else if (strcmp(cmd, "ls") == 0) {
                wrapper(cmdls, tokens);
            } else if (strcmp(cmd, "cd") == 0) {
                wrapper(cmdCd, tokens);
            } else {
                printf("Error: unknown command '%s'.\n", cmd);
            }
        }

        free(input);
        freeTokens(tokens);
    }

    fclose(fp);
    return 0;
}