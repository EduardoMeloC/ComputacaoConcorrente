/* Prof.: Silvana Rosseto */
/* Módulo 2 - Laboratório: 4 */
/* Aluno.: Eduardo Melo */
/* Código: Imprimir em T4 antes de T2 e T3. A ordem de T2 e T3 não importa. 
 *          T2 e T3 imprimem antes de T1. Utilizar semáforos */

/*
 * Comentários:
 *      > Decidi reciclar a ideia de usar o shuffle para deixar a ordem das funções menos previsíveis
 *      > Fazer esse trabalho com semáforo foi bem mais fácil :)
*/      

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

// Variáveis globais para os semáforos
sem_t T2_condt1, T3_condt1, T4_condt2t3; 

void* T1(void* args){

    // espera T2 e T3 para executar
    sem_wait(&T2_condt1); 
    sem_wait(&T3_condt1);

    printf("Volte sempre!\n");

    pthread_exit(NULL);
}

void* T2(void* args){

    sem_wait(&T4_condt2t3); // espera T4 executar
    printf("Fique a vontade.\n");
    sem_post(&T2_condt1); // permite T1 executar

    pthread_exit(NULL);
}

void* T3(void* args){
    
    sem_wait(&T4_condt2t3); // espera T4 executar
    printf("Sente-se por favor.\n");
    sem_post(&T3_condt1); // permite T1 executar

    pthread_exit(NULL);
}

void* T4(void* args){

    printf("Seja bem-vindo!\n");
    sem_post(&T4_condt2t3); // permite que T2/T3 executem
    sem_post(&T4_condt2t3); // permite que T2/T3 executem

    pthread_exit(NULL);
}

static void shuffle(void *array, size_t n, size_t size) {
    // Retirado do Stackoverflow
    // https://stackoverflow.com/questions/6127503/shuffle-array-in-c
    char tmp[size];
    char *arr = array;
    size_t stride = size * sizeof(char);

    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; ++i) {
            size_t rnd = (size_t) rand();
            size_t j = i + rnd / (RAND_MAX / (n - i) + 1);

            memcpy(tmp, arr + j * stride, size);
            memcpy(arr + j * stride, arr + i * stride, size);
            memcpy(arr + i * stride, tmp, size);
        }
    }
}

int main(void){
    pthread_t tid[4];
    // Coloca as funções das threads dentro de um vetor em ordem aleatoria
    void* (*Tarr[])(void*) = {T1, T2, T3, T4};
    shuffle(Tarr, 4, sizeof(Tarr[0]));

    // Inicia os Semaforos
    sem_init(&T2_condt1, 0, 0);
    sem_init(&T3_condt1, 0, 0);
    sem_init(&T4_condt2t3, 0, 0);

    // cria threads
    for(int i=0; i < 4; i++){
        if(pthread_create(tid, NULL, Tarr[i], NULL)) { printf("Erro\n"); exit(EXIT_FAILURE); }
    }

    // desaloca variaveis e termina
    sem_destroy(&T2_condt1);
    sem_destroy(&T3_condt1);
    sem_destroy(&T4_condt2t3);
    pthread_exit(NULL);
}
