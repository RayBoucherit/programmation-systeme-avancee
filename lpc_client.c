#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include "lpc_struct.h"
#include "lpc_client.h"
#include "lpc_serveur.h"
#include "lpc_fonction.h"

lpc_string *lpc_make_string(const char *s, int taille){
    if(taille > 0 && s == NULL){
        lpc_string *lpc = malloc(sizeof(lpc_string) + taille);
        if(lpc == NULL){
            perror("malloc");
            exit(1);
        }
        memset(lpc, 0, sizeof(lpc_string) + taille);
        lpc->slen = taille;
        memset(lpc->string, '\0', sizeof(char));
        return lpc;
    }
    if(taille <= 0 && s != NULL){
        int len = strlen(s) + 1;
        lpc_string* lpc = malloc(sizeof(lpc_string) + len);
        if(lpc == NULL){
            perror("malloc");
            exit(1);
        }
        memset(lpc, 0, sizeof(lpc_string) + len);
        strncpy(lpc->string, s, len);
        lpc->slen = len;
        return lpc;
    }
    if(taille >= strlen(s) + 1){
        lpc_string* lpc = malloc(sizeof(lpc_string) + taille);
        if(lpc == NULL){
            perror("malloc");
            exit(1);
        }
        memset(lpc, 0, sizeof(lpc_string) + taille);
        strncpy(lpc->string, s, strlen(s) + 1);
        lpc->slen = taille;
        return lpc;
    }
    else{
        return NULL;
    }
}

void *lpc_open(const char *name){
    //fonction appelée par un processus client souhaitant appeler des fonctions du serveur
    struct stat st;
    int fd = shm_open(name, O_RDWR , 0);
    if(fd < 0){
        if(errno == ENOENT){
            return NULL;
        }
        perror("shm_open");
        exit(1);
    }
    if(fstat(fd, &st) < 0){
        perror("fstat");
        exit(1);
    }
    header *h = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0);
    if(h == MAP_FAILED){
        perror("mmap");
        exit(1);
    }
    h->size = st.st_size;
    return h;
}

int lpc_close(void *mem){
    //fonction appelée par un client lorsqu’il ne veut plus faire d’appels aux fonctions du serveur.
    if(mem != NULL){
        header *h = (header *)mem;
        if(munmap(mem, h->size + sizeof(header)) < 0){
            perror("munmap");
            exit(1);
        }
        return 1;
    }
    printf("in lpc_close : memory is NULL\n");
    return -1;
}
/*
int lpc_call(void *memory, const char *fun_name, ...){
    //faire appel à une fonction du serveur
    if(memory != NULL){
        //variables nécessaires au switch et au fonctionnement de va_arg
        va_list args;
        va_start(args, fun_name);
        lpc_type lpt;
        lpc_string *str;
        int *i;
        double *d;
        int pointer = 0;
        //memoire
        header *h = (header *)memory;
        //nom de la fonction a appelle
        strcpy(h->fun_name, fun_name);
        //code retour fonctions pthread
        int code;
        //initialisation de la mémoire avant l'appel de fonction
        do{
            lpt = va_arg(args, lpc_type);
            switch(lpt){
                case STRING:
                str = va_arg(args, lpc_string *);
                strncpy(&(h->DATA[pointer]), str->string, str->slen);
                pointer += sizeof(lpc_string) + str->slen;
                break;
                case DOUBLE:
                d = va_arg(args, double *);
                memset(&(h->DATA[pointer]), *d, sizeof(double));
                pointer += sizeof(double);
                break;
                case INT:
                i = va_arg(args, int *);
                memset(&(h->DATA[pointer]), *i, sizeof(int));
                pointer += sizeof(int);
                break;
                default:
                break;
            }
        }while(lpt != NOP);
        code = pthread_mutex_lock(&h->mutex);
        if(code != 0){
            printf("%s\n", strerror(code));
            exit(1);
        }
        //attendre un message
        while(h->flag){
            code = pthread_cond_wait(&h->rcond, &h->mutex);
            if(code != 0){
                printf("%s\n", strerror(code));
                exit(1);
            }
        }
        h->flag = 1;
        if(code != 0){
            printf("%s\n", strerror(code));
            exit(1);
        }
        code = pthread_mutex_unlock(&h->mutex);
        if(code != 0){
            printf("%s\n", strerror(code));
            exit(1);
        }
        code = pthread_cond_signal(&h->wcond);
        if(code != 0){
            printf("%s\n", strerror(code));
            exit(1);
        }
        return 0;
    }
    else{
        printf("memory is NULL\n");
        return -1;
    }
}
*/

int lpc_call(void *memory, const char *fun_name, ...){
    //faire appel à une fonction du serveur
    if(memory != NULL){
        //variables nécessaires au switch et au fonctionnement de va_arg
        va_list args;
        va_start(args, fun_name);
        lpc_type lpt;
        lpc_string *str;
        int *i;
        double *d;
        int pointer = 0;
        //memoire
        header *h = (header *)memory;
        //nom de la fonction a appelle
        strcpy(h->fun_name, fun_name);
        //code retour fonctions pthread
        int code;
        code = pthread_mutex_lock(h->mutex);
        if(code != 0){
            printf("%s\n", strerror(code));
            exit(1);
        }
        pthread_mutex_lock(h->mutex);
        while(!h->flag){
            pthread_cond_wait(h->wcond, h->mutex);
        }
        //initialisation de la mémoire avant l'appel de fonction
        do{
            lpt = va_arg(args, lpc_type);
            switch(lpt){
                case STRING:
                str = va_arg(args, lpc_string *);
                strncpy(h->DATA + pointer, str->string, str->slen);
                pointer += sizeof(lpc_string) + str->slen;
                break;
                case DOUBLE:
                d = va_arg(args, double *);
                memcpy(h->DATA + pointer, d, sizeof(double));
                pointer += sizeof(double);
                break;
                case INT:
                i = va_arg(args, int *);
                memcpy(h->DATA + pointer, i, sizeof(int));
                pointer += sizeof(int);
                break;
                default:
                break;
            }
        }while(lpt != NOP);
        h->flag = 0;
        code = pthread_mutex_unlock(h->mutex);
        if(code != 0){
            printf("%s\n", strerror(code));
            exit(1);
        }
        code = pthread_cond_signal(h->rcond);
        if(code != 0){
            printf("%s\n", strerror(code));
            exit(1);
        }
        return 0;
    }
    printf("in lpc_call : memory is null");
    return -1;
}

int main(int argc, char *argv[]){
    int fd = shm_open(NAME, O_RDWR | O_CREAT, 0666);
    if(fd < 0){
        perror("shm_open");
        exit(1);
    }
    if(ftruncate(fd, SIZE) < 0){
        perror("ftruncate");
        exit(1);
    }
    header *h = lpc_open(NAME);
    if(h == NULL){
        printf("in main : memory is NULL\n");
        return 0;
    }
    char fun_name[NAMELEN];
    char arg1[NAMELEN];
    char arg2[NAMELEN];
    int i = -1;
    printf("Quelle fonction souhaitez vous appeler (sum, sub, mult, div, concat)?\n");
    scanf("%s", fun_name);
    if(!memcmp(fun_name, "divs", 3)){
        int a, b;
        printf("premier argument : \n");
        scanf("%s", arg1);
        printf("deuxième argument : \n");
        scanf("%s", arg2);
        a = atoi(arg1);
        b = atoi(arg2);
        lpc_call(h, fun_name, INT, &a, INT, &b, NOP);
        a = *(int *)h->DATA;
        b = *(int *)(&h->DATA[sizeof(int)]);
        printf("arg1 : %d, arg2 : %d\n", a, b);
        divs(h);
        a = *(int *)h->DATA;
        printf("result : %d\n", a);
    }
    lpc_close(h);
    shm_unlink(NAME);
}   