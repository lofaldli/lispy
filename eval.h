#ifndef EVAL_H
#define EVAL_H

#include "types.h"

lval* lval_eval_sexpr(lenv* e, lval* v);
lval* lval_eval(lenv* e, lval* v);
lval* lval_call(lenv* e, lval* f, lval* a);

#endif
