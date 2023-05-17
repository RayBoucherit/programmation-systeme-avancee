#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include "lpc_fonction.h"
#include "lpc_struct.h"
#include "lpc_serveur.h"

static int sizeof_header(int size){
    return size - sizeof(header);
}

static int init_mutex(pthread_mutex_t *pmutex){
    pthread_mutexattr_t mutexattr;   
    int code;   
    code = pthread_mutexattr_init(&mutexattr);       
    if( code != 0 ){  
        return code;
    }   
    code = pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED);   
    if( code != 0 ){             
        return code;
    }   
    code = pthread_mutex_init(pmutex, &mutexattr);   
    return code; 
}

static int init_cond(pthread_cond_t *pcond){   
    pthread_condattr_t condattr;   
    int code;   
    code = pthread_condattr_init( &condattr ) ;   
    if( code != 0 ){
        return code;
    }
    code = pthread_condattr_setpshared(&condattr, PTHREAD_PROCESS_SHARED);   
    if( code != 0 ){
        return code;
    }
    return pthread_cond_init( pcond, &condattr );  
}

lpc_function** set_fun(){
    lpc_function **set = malloc(FUNC_NUM * sizeof(lpc_function));
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

static fun get_fun(lpc_function *func[], const char *name){
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

static void exec_fun(void *memory, fun f){
    if(memory != NULL){
        header *h = (header *)memory;
        int v = f(memory);
        h->value = v;
        //TODO set h->err
    }
}

void *lpc_create(const char *nom, size_t capacite){
    int code;
    shm_unlink(nom);
    int fd = shm_open(nom, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(fd < 0){
        perror("shm_open");
        return NULL;
    }
    int size = sysconf(_SC_PAGESIZE) * capacite;
    if(ftruncate(fd, size) < 0){
        perror("ftruncate");
        return NULL;
    }
    header *h = (header *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(h == NULL){
        perror("mmap");
        return NULL;
    }
    memset((void *)h, 0, size);
    h->size = sizeof_header(size);
    //initialement memoire libre
    h->flag = 1;
    code = init_mutex(&h->mutex);
    if(code != 0){
        printf("%s\n", strerror(code));
        exit(1);
    }
    code = init_cond(&h->rcond);
    if(code != 0){
        printf("%s\n", strerror(code));
        exit(1);
    }
    code = init_cond(&h->wcond);
    if(code != 0){
        printf("%s\n", strerror(code));
        exit(1);
    }
    //initialise la position de DATA dans la memoire
    char *pointer = (char *)h;
    pointer = pointer + sizeof(header);
    h->DATA = pointer;
    printf("%p\n", h->DATA);
    return (void *)h;
}

int main(int argc, char *argv[]){
    header *h = lpc_create(NAME, SIZE);
    while(1){
        int code = pthread_mutex_lock(&h->mutex);
        if(code != 0){
            printf("%s\n", strerror(code));
            exit(1);
        }
        while(!h->flag){
            code = pthread_cond_wait(&h->wcond, &h->mutex);
            if(code != 0){
                printf("%s\n", strerror(code));
                exit(1);
            }
        }
        exec_fun(h, sum);
        code = pthread_mutex_unlock(&h->mutex);
        if(code != 0){
            printf("%s\n", strerror(code));
            exit(1);
        }
        code = pthread_cond_signal(&h->rcond);
        if(code != 0){
            printf("%s\n", strerror(code));
            exit(1);
        }
    }
}