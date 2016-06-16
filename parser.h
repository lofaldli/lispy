#ifndef PARSER_H
#define PARSER_H

#include "types.h"
#include "mpc.h"

void init_parser(void);
void free_parser(void);

lval* parse(char* input);
void parse_args(lenv* e, int argc, char** argv);

lval* lval_read_num(mpc_ast_t* t);
lval* lval_read_str(mpc_ast_t* t);
lval* lval_read_file(char* filename);
lval* lval_read(mpc_ast_t* t);

#endif
