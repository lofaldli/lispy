#include <string.h>
#include "eval.h"
#include "types.h"
#include "builtin.h"

/**
 * evaluate s-expression
 */
lval* lval_eval_sexpr(lenv* e, lval* v) {

    /* eval children */
    for (int i=0; i<v->count; i++) {
        v->cell[i] = lval_eval(e, v->cell[i]);
    }

    /* check for errors */
    for (int i=0; i < v->count; i++) {
        if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }
    }

    /* empty expr */
    if (v->count == 0) { return v; }
    /* single expr */
    if (v->count == 1) { return lval_take(v, 0); }

    /* ensure first element is symbol */
    lval* f = lval_pop(v, 0);
    if (f->type != LVAL_FUN) {
        lval* err = lval_err(
                "s-expr starts with incorrect type. expected %s, got %s",
                ltype_name(LVAL_FUN), ltype_name(f->type));
        lval_del(f); lval_del(v);
        return err;
    }

    /* call builtin function */
    lval* result = lval_call(e, f, v);
    lval_del(f);
    return result;
}

/**
 * generic evaluation
 */
lval* lval_eval(lenv* e, lval* v) {
    if (v->type == LVAL_SYM) {
        lval* x = lenv_get(e, v);
        lval_del(v);
        return x;
    }
    if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(e, v); }
    return v;
}

/**
 * make function call
 */
lval* lval_call(lenv* e, lval* f, lval* a) {
    if (f->builtin) { return f->builtin(e, a); }

    int given = a->count;
    int total = f->formals->count;

    while (a->count) {

        /* no more arguments to bind */
        if (f->formals->count == 0) {
            lval_del(a);
            return lval_err(
                "function passed too many arguments. expected %i, got %i",
                total, given);
        }
        /* get next formal symbol */
        lval* sym = lval_pop(f->formals, 0);

        if (strcmp(sym->sym, "&") == 0) {
            /* & must be followed by more symbols */
            if (f->formals->count != 1) {
                lval_del(a);
                return lval_err("function format invalid. \
                        '&' must be followed by at least one symbol");
            }
            /* next formal should be bound to remaining arguments */
            lval* nsym = lval_pop(f->formals, 0);
            lenv_put(f->env, nsym, builtin_list(e, a));
            lval_del(sym); lval_del(nsym);
            break;
        }

        /* get next argument from list */
        lval* val = lval_pop(a, 0);
        /* bind copy into function environment */
        lenv_put(f->env, sym, val);
        /* delete symbol and value */
        lval_del(sym); lval_del(val);

    }

    lval_del(a);

    /* if '&' remains to bind to empty list */
    if (f->formals->count > 0 &&
            strcmp(f->formals->cell[0]->sym, "&") == 0) {

        if (f->formals->count != 2) {
            return lval_err("function format invalid.\
                    '&' most be followed by at least one symbol");
        }

        /* pop and delete '&' */
        lval_del(lval_pop(f->formals, 0));
        /* pop next symbol and create empty list */
        lval* sym = lval_pop(f->formals, 0);
        lval* val = lval_qexpr();
        /* bind to environment and delete */
        lenv_put(f->env, sym, val);
        lval_del(sym); lval_del(val);
    }

    /* all formals have been bound */
    if (f->formals->count == 0) {

        f->env->par = e;
        return builtin_eval(f->env,
                lval_add(lval_sexpr(), lval_copy(f->body)));
    } else {
        return lval_copy(f);
    }
}
