#include "mpc.h"
#include "eval.h"
#include "types.h"
#include "parser.h"
#include "builtin.h"

mpc_parser_t* Number;
mpc_parser_t* Symbol;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* Program;

void init_parser(void) {

    Number  = mpc_new("number");
    Symbol  = mpc_new("symbol");
    String  = mpc_new("string");
    Comment = mpc_new("comment");
    Sexpr   = mpc_new("sexpr");
    Qexpr   = mpc_new("qexpr");
    Expr    = mpc_new("expr");
    Program = mpc_new("program");

    mpca_lang(MPCA_LANG_DEFAULT,
            "                                                \
                number  : /-?[0-9]+/ ;                       \
                symbol  : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ; \
                string  : /\"(\\\\.|[^\"])*\"/ ;             \
                comment : /;[^\\r\\n]*/ ;                    \
                sexpr   : '(' <expr>* ')' ;                  \
                qexpr   : '{' <expr>* '}' ;                  \
                expr    : <number> | <symbol> | <string>     \
                        | <comment> | <sexpr> | <qexpr> ;    \
                program : /^/ <expr>* /$/ ;                  \
            ",
            Number, Symbol, String, Comment,
            Sexpr, Qexpr, Expr, Program);
}

void free_parser(void) {
    mpc_cleanup(8,
            Number, Symbol, String, Comment,
            Sexpr, Qexpr, Expr, Program);
}

lval* parse(char* input) {
    mpc_result_t r;
    lval* x = NULL;
    if (mpc_parse("<stdin>", input, Program, &r)) {
        x = lval_read(r.output);
        mpc_ast_delete(r.output);
    } else {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
    }
    return x;
}

void parse_args(lenv* e, int argc, char** argv) {
    if (argc >= 2) {
        for (int i=1; i<argc; i++) {
            lval* args = lval_add(lval_sexpr(), lval_str(argv[i]));
            lval* x = builtin_load(e, args);

            if (x->type == LVAL_ERR) { lval_println(x); }
            lval_del(x);
        }
    }
}

/**
 * convert ast to number
 */
lval* lval_read_num(mpc_ast_t* t) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE
        ? lval_num(x)
        : lval_err("invalid number");
}

lval* lval_read_str(mpc_ast_t* t) {
    /* remove last quote character */
    t->contents[strlen(t->contents) - 1] = '\0';
    /* copy string w/o first quote character */
    char* unescaped = malloc(strlen(t->contents+1)+1);
    strcpy(unescaped, t->contents+1);
    unescaped = mpcf_unescape(unescaped);
    lval* str = lval_str(unescaped);

    free(unescaped);
    return str;
}

lval* lval_read_file(char* filename) {
    mpc_result_t r;
    if (mpc_parse_contents(filename, Program, &r)) {
        lval* x = lval_read(r.output);
        mpc_ast_delete(r.output);
        return x;
    } else {
        char* err_msg = mpc_err_string(r.error);
        mpc_err_delete(r.error);

        lval* err = lval_err("could not load library %s", err_msg);
        free(err_msg);
        return err;
    }
}

/**
 * convert ast to value
 */
lval* lval_read(mpc_ast_t* t) {

    if (strstr(t->tag, "number")) { return lval_read_num(t); }
    if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }
    if (strstr(t->tag, "string")) { return lval_read_str(t); }

    lval* x = NULL;
    if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
    if (strstr(t->tag, "sexpr"))  { x = lval_sexpr(); }
    if (strstr(t->tag, "qexpr"))  { x = lval_qexpr(); }

    for (int i=0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
        if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
        if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }
        if (strstr(t->children[i]->tag, "comment")) { continue; }
        x = lval_add(x, lval_read(t->children[i]));
    }
    return x;
}
