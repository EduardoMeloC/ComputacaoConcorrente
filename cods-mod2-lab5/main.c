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
 *      > Eu decidi fazer os testes automaticos em um outro arquivo, main_test.c
 *      > Aqui, voce consegue mudar g_vec e ler informacoes no stdout
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "safe.c"

// Vetor utilizado para entrada/saida
int g_vec[] = { 378884430, 110556248 };

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

// funcao barreira
void barrier(int nthreads){
    pthread_mutex_lock(&x_mutex);
    g_blocked++;
    if( g_blocked < nthreads ){
        pthread_cond_wait(&x_cond, &x_mutex);;
    }
    else{
        g_blocked = 0;
        printf("SAIU DA BARREIRA\n");
        print_vec(g_vec, nthreads);
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
            printf("T%ld (%ld - %d) -->> vai ler posicao %ld : aux = %d\n", id, id, i, id-i, aux);
        }
        else{
            printf("T%ld (%ld - %i) -->> posicao invalida, nao faz nada\n", id, id, i);
        }
        barrier(g_nthreads);
        printf("T%ld escrevendo...\n", id);
        g_vec[id] = aux + g_vec[id];
        aux = 0;
        barrier(g_nthreads);
    }

    pthread_exit(NULL);
}

// fluxo principal
int main(int argc, char* argv[]){
    int vec_len = sizeof(g_vec) / sizeof(g_vec[0]); // tamanho do g_vec
    g_nthreads = vec_len;
    int* vec_seq = (int*) safe_malloc(sizeof(int)*vec_len); // usado na versao sequencial
    memcpy(vec_seq, g_vec, sizeof(g_vec));
    int* vec_clone = (int*) safe_malloc(sizeof(int)*vec_len); // usado na impressao final
    memcpy(vec_clone, g_vec, sizeof(g_vec));
    pthread_t* threads = (pthread_t*) safe_malloc(sizeof(pthread_t)*vec_len);

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

    // exibe resultados
    printf("Entrada (g_vec):\t");
    print_vec(vec_clone, vec_len);
    printf("\nSaida Sequencial:\t");
    print_vec(vec_seq, vec_len);
    printf("Saida Concorrente:\t");
    print_vec(g_vec, vec_len);
}
