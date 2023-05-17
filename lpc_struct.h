#ifndef LPC_STRUCT_H
#define LPC_STRUCT_H

#include <bits/pthreadtypes.h>
#include <pthread.h>

#define NAME "/nimportequoi"
#define SIZE 128
#define NAMELEN 48
#define FUNC_NUM 5

typedef enum{STRING, DOUBLE, INT, NOP}  lpc_type;

typedef struct{
    int slen;
    char string[];
}lpc_string;

typedef struct{
    //header contient
    //une valeur de retour
    int value;
    //une valeur d'erreur
    int err;
    //pid du dernier processus
    int pid;
    //la taille totale de la structure
    int size;
    //la condition :
    //si le producteur peut ecrire une donnee : 0
    //si le consommateur peut lire une donnee : 1
    int flag;
    //une valeur mutex
    pthread_mutex_t *mutex;
    //une valeur cond pour la lecture
    pthread_cond_t *rcond;
    //une valeur cond pour l'ecriture
    pthread_cond_t *wcond;
    //nom function
    char fun_name[NAMELEN];
    //un pointeur vers DATA
    char DATA[];
}header;

typedef int (*fun) (void *); 
typedef struct{
    char fun_name[NAMELEN];
    fun f;
}lpc_function;

#endif