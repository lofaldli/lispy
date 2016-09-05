#include <stdio.h>

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer)+1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy)-1] = '\0';
    return cpy;
}
void add_history(char* unused) {}

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

void say_hello(void) {
    puts("my-own-lisp version 0.0.0.0.8");
    puts("press ctrl+c to exit \n");
}

char* prompt(void) {
    char* input = readline("> ");
    add_history(input);
    return input;
}
