/* Prof.: Silvana Rosseto */
/* Módulo 2 - Laboratório: 4 */
/* Aluno.: Eduardo Melo */
/* Código: Imprimir em T4 antes de T2 e T3. A ordem de T2 e T3 não importa. T2 e T3 imprimem antes de T1 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

/*
 * Comentários:
 *      > Confesso que tive dificuldade ao longo do trabalho. É possível que eu tenha errado algo ou não tenha tomado a solução adequada, mas não obtive erros ao executar a solução que encontrei.
 *      > Eu quis brincar com ponteiro de função pra fazer um for que chama as threads, lá na main.
 *      > No lugar de colocar um for pra "gastar tempo", tal qual foi feito nos exemplos, eu escolhi dar um shuffle nesse vetor de ponteiros de função. Dessa forma, a ordem de execução das threads é menos previsível.
*/      

unsigned x = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

void* T1(void* args){
    pthread_mutex_lock(&x_mutex);
    while(x < 3){
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex);

    printf("Volte sempre!\n");

    pthread_exit(NULL);
}
void* T2(void* args){
    pthread_mutex_lock(&x_mutex);
    if(x == 0){
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    printf("Fique a vontade.\n");
    x++;
    if(x == 3){
        pthread_cond_signal(&x_cond);
    }
    pthread_mutex_unlock(&x_mutex);

    pthread_exit(NULL);
}
void* T3(void* args){
    pthread_mutex_lock(&x_mutex);
    if(x == 0){
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    printf("Sente-se por favor.\n");
    x++;
    if(x == 3){
        pthread_cond_signal(&x_cond);
    }
    pthread_mutex_unlock(&x_mutex);

    pthread_exit(NULL);
}
void* T4(void* args){
    printf("Seja bem-vindo!\n");

    pthread_mutex_lock(&x_mutex);
    {
        x++;
        pthread_cond_broadcast(&x_cond);
    }
    pthread_mutex_unlock(&x_mutex); 

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
    void* (*Tarr[])(void*) = {T1, T2, T3, T4};
    shuffle(Tarr, 4, sizeof(Tarr[0]));

    // inicializa mutex e variavel de condicao
    pthread_mutex_init(&x_mutex, NULL);
    pthread_cond_init(&x_cond, NULL);

    // cria threads
    for(int i=0; i < 4; i++){
        if(pthread_create(tid, NULL, Tarr[i], NULL)) { printf("Erro\n"); exit(EXIT_FAILURE); }
    }

    // desaloca variaveis e termina
    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond);
    pthread_exit(NULL);
}
