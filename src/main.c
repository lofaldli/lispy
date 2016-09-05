#include <stdlib.h>

#include "parser.h"
#include "types.h"
#include "eval.h"
#include "builtin.h"
#include "prompt.h"

int main(int argc, char** argv) {

    init_parser();

    /* initialize environment */
    lenv* e = lenv_new();
    lenv_add_builtins(e);

    /* load standard library functions */
    lval* x = builtin_load(e, lval_add(lval_sexpr(), lval_str("prelude.lsp")));
    if (x->type == LVAL_ERR) { lval_println(x); }
    lval_del(x);

    if (argc >= 2) {

        /* read from command line */
        parse_args(e, argc, argv);

    } else {

        /* interactive prompt */
        say_hello();
        while (1) {
            char* input = prompt();
            lval* x = parse(input);
            if (x != NULL) {
                x = lval_eval(e, x);
                lval_println(x);
                lval_del(x);
            }
            free(input);
        }

    }

    /* cleanup */
    lenv_del(e);
    free_parser();

    return 0;
}
