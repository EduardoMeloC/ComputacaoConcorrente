/* Prof.: Silvana Rosseto */
/* Módulo 1 - Laboratóio: 1 */
/* Aluno.: Eduardo Melo */
/* Código: Incrementa de 1 cada elemento de um vetor de 10000 elementos */

/* 
 * Comentários:
 *      Eu escolhi criar uma thread que percorre do início do vetor até o meio,
 *      e outra do meio até o fim.
 *
 *      Eu detesto printar erro sempre que quero fazer malloc, então peguei uma função
 *      pra me ajudar. Eu teria feito isso pras funções do pthread, mas parecia esforço demais.
 *
 *      Não me preocupei com os casos em que ARRAY_SIZE e N_THREADS possuem valores diferentes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARRAY_SIZE 10000
#define NTHREADS 2

// protótipos de funcoes auxiliares
void* safe_calloc(size_t nmemb, size_t size);
void* safe_malloc(size_t n);

// receberá o vetor por onde iremos iterar
int* g_array;

// cria estrutura de dados para armazenar os argumentos da thread
typedef struct {
    int start_i,
        end_i,
        increment;
} ThreadArguments;

// funcao executada pelas threads
void* increment_array (void* arg){
    ThreadArguments* args = (ThreadArguments*) arg;
    int start_i = args->start_i;
    int end_i = args->end_i;
    int increment = args->increment;
    free(arg);

    for(int i = start_i; i < end_i; i += increment) {
        g_array[i] += 1;
    }

    pthread_exit(NULL);
}

// Funcao principal do programa
int main(void) {
    g_array = (int*) safe_calloc(ARRAY_SIZE, sizeof(int)); // aloca memória para g_array 
    pthread_t tids[2]; // identificadores das threads no sistema 
    ThreadArguments *arg; // receberá os argumentos para a thread

    for(int i = 0; i < NTHREADS; i++){
        arg = safe_malloc(sizeof(ThreadArguments));
        arg->start_i = ARRAY_SIZE / NTHREADS * i;
        arg->end_i = ARRAY_SIZE / NTHREADS * (i+1);
        arg->increment = 1;

        if(pthread_create(&tids[i], NULL, increment_array, (void*) arg)){
            fprintf(stderr, "ERROR: fail on pthread_create().\n");
            exit(EXIT_FAILURE);
        }
    }

    // Espera as threads terminarem
    for(int i = 0; i < NTHREADS; i++) {
        if(pthread_join(tids[i], NULL)){
            fprintf(stderr, "ERROR: fail on pthread_join().\n");
            exit(EXIT_FAILURE);
        }
    }

    // Verifica se todos os elementos do vetor são iguais a 1
    for(int i = 0; i < ARRAY_SIZE; i++){
        if(g_array[i] != 1){
            printf("Unexpected output at index %d. Obtained value: %d.\n", i, g_array[i]);
            return -1;
        }
    }
    printf("Yay! :D\n");

    pthread_exit(NULL);
}

// Funcoes auxiliares
void* safe_calloc(size_t nmemb, size_t size){
    void *p = calloc(nmemb, size);
    if(p == NULL) {
        fprintf(stderr, "ERROR: failed to allocate %zu bytes.\n", size);
        exit(EXIT_FAILURE);
    }
    return p;
}

void* safe_malloc(size_t size){
    void *p = malloc(size);
    if(p == NULL) {
        fprintf(stderr, "ERROR: failed to allocate %zu bytes.\n", size);
        exit(EXIT_FAILURE);
    }
    return p;
}

