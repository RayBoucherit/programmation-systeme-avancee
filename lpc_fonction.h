#ifndef LPC_FUNC_H
#define LPC_FUNC_H

#define SUM "sum"
#define SUB "sub"
#define MULT "mult"
#define DIVS "divs"
#define CAT "concat"

#include "lpc_struct.h"

lpc_function** set_fun();
fun get_fun(lpc_function **func, const char *name);
int is_valid(lpc_function **func, const char *name);

//int *, int * -> int
int sum(void *mem);
int sub(void *mem);
int mult(void *mem);
int divs(void *mem);

//string *, string * -> string *
int concat(void *mem);

#endif