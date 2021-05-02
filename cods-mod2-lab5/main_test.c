/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 1 - Laboratório: 1 */
/* Codigo: Somas parciais em cada posicao de um vetor */
/* Exemplo: 
 *      Entrada: [1, 4, -1, 7]
 *      Saída:   [1, 5, 4, 11]
 */

/*
 * Comentarios: 
 *      > Eu decidi fazer os testes automaticos neste arquivo.
 *      > As funcoes de barreira e das threads sao iguais.
 *      > Aqui, eu rodo o programa 10000x comparando com a versao sequencial
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "safe.c"

// minha CPU tem 4 cores, eu uso esse numero pra gerar o tamanho do vetor
#define MAX_NTHREADS 4

#define N_TESTS 10000

// Vetor utilizado para entrada/saida
int* g_vec;

// Outras variaveis
int g_blocked, g_nthreads;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

// funcao para imprimir vetor de int
void print_vec(int* vec, int vec_len){
    printf("[");
    for (int i = 0; i< vec_len-1; i++)
        printf("%d, ", vec[i]);
    printf("%d]\n", vec[vec_len-1]);
}
//
// funcao barreira
void barrier(int nthreads){
    pthread_mutex_lock(&x_mutex);
    g_blocked++;
    if( g_blocked < nthreads ){
        pthread_cond_wait(&x_cond, &x_mutex);;
    }
    else{
        g_blocked = 0;
        pthread_cond_broadcast(&x_cond);
    }
    pthread_mutex_unlock(&x_mutex);
}

// funcao executada pelas threads
void* Tfunc(void* arg){
    long int id = (long int) arg;
    int aux = 0;

    for(int i = 1; i < g_nthreads; i*=2){
        if(id - i >= 0){
            aux = g_vec[id - i];
        }
        else{
        }
        barrier(g_nthreads);
        g_vec[id] = aux + g_vec[id];
        aux = 0;
        barrier(g_nthreads);
    }

    pthread_exit(NULL);
}

// fluxo principal
int main(int argc, char* argv[]){
    srand(time(NULL));
    g_vec = (int*) safe_malloc(MAX_NTHREADS * sizeof(int)); // usado pelas threads
    int* vec_seq = (int*) safe_malloc(MAX_NTHREADS * sizeof(int)); // usado na versao sequencial
    int* vec_clone = (int*) safe_malloc(MAX_NTHREADS * sizeof(int)); // usado ao obter erro
    pthread_t* threads = (pthread_t*) safe_malloc(MAX_NTHREADS * sizeof(pthread_t));

    for(int it = 0; it < N_TESTS; it++){
        // cria vec aleatorio
        int vec_len = pow(2, rand()%(MAX_NTHREADS-1));
        for (int i = 0; i < vec_len; i++){
            g_vec[i] = rand() % 1000;
            vec_seq[i] = g_vec[i];
            vec_clone[i] = g_vec[i];
        }

        g_nthreads = vec_len;
        memcpy(vec_seq, g_vec, sizeof(int) * vec_len);

        // versao sequencial
        for (int i = 1; i < vec_len; i++)
            vec_seq[i] = vec_seq[i] + vec_seq[i-1];

        // versao concorrente
        for(long int i = 0; i < vec_len; i++){
            safe_pthread_create(&threads[i], NULL, Tfunc, (void*)i);
        }

        // aguarda threads
        for (int i = 0; i < vec_len; i++){
            safe_pthread_join(threads[i], NULL);
        }

        // compara resultados
        for (int i = 0; i < vec_len; i++){
            if (g_vec[i] != vec_seq[i]){
                printf("Resultado inesperado foi encontrado (Iteracao %d):\n", it);
                printf("Entrada: \t");
                print_vec(vec_clone, vec_len);
                printf("Sequencial: \t");
                print_vec(vec_seq, vec_len);
                printf("Concorrente: \t");
                print_vec(g_vec, vec_len);

                exit(EXIT_FAILURE);
            }
        }

    }

    printf("Foram feitos %d testes sem erros.\n", N_TESTS);

}
