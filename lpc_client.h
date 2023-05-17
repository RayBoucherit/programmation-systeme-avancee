#ifndef LPC_CL_H
#define LPC_CL_H

lpc_string *lpc_make_string(const char *s, int taille);
void *lpc_open(const char *name);
int lpc_close(void *mem);
int lpc_call(void *memory, const char *fun_name, ...);

#endif