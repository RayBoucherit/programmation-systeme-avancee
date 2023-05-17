#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lpc_struct.h"
#include "lpc_client.h"
#include "lpc_fonction.h"

lpc_function** set_fun(){
    //SEGFAULT : ligne 17
    lpc_function **set = malloc(FUNC_NUM * sizeof(lpc_function));
    if(set == NULL){
        perror("malloc");
        exit(1);
    }
    for(int i = 0; i < FUNC_NUM; i++){
        switch(i){
            case 0: 
                strcpy(set[0]->fun_name, SUM);
                set[0]->f = sum;
            break;
            case 1:
                strcpy(set[1]->fun_name, SUB);
                set[1]->f = sub;
            break;
            case 2:
                strcpy(set[2]->fun_name, MULT);
                set[2]->f = mult;
            break;
            case 3:
                strcpy(set[3]->fun_name, DIVS);
                set[3]->f = divs;
            break;
            case 4:
                strcpy(set[4]->fun_name, CAT);
                set[4]->f = concat;
            break;
        }
    }
    return set;
}

fun get_fun(lpc_function *func[], const char *name){
    //retourne la fonction de nom "name" ou NULL
    if(func != NULL){
        lpc_function *tmp;
        int i = 0;
        do{
            tmp = func[i];
            if(strcmp(tmp->fun_name, name) == 0){
                return tmp->f;
            }
            i++;
        }while(tmp != NULL);
        return NULL;
    }
    return NULL;
}

int is_valid(lpc_function **func, const char *name){
    //retourne l'indice de la fonction de nom "name" ou -1
    if(func != NULL){
        lpc_function *tmp;
        int i = 0;
        do{
            tmp = func[i];
            if(strcmp(tmp->fun_name, name) == 0){
                return i;
            }
            i++;
        }while(tmp != NULL);
        return -1;
    }
    return -1;
}

int sum(void *mem){
    header *h = (header *)mem;
    int *a = (int *)h->DATA;
    int *b = a + 1;
    *a = *a + *b;
    *b = 0;
    h->value = 0;
    return 0;
}

int sub(void *mem){
    header *h = (header *)mem;
    int *a = (int *)h->DATA;
    int *b = a + 1;
    *a = *a - *b;
    *b = 0;
    h->value = 0;
    return 0;
}

int mult(void *mem){
    header *h = (header *)mem;
    int *a = (int *)h->DATA;
    int *b = a + 1;
    *a = *a * *b;
    *b = 0;
    h->value = 0;
    return 0;
}

int divs(void *mem){
    header *h = (header *)mem;
    int *a = (int *)h->DATA;    
    int *b = a + 1;
    *a = *a / *b;
    *b = 0;
    h->value = 0;
    return 0;
}

int concat(void *mem){
    //TODO
    return 0;
}