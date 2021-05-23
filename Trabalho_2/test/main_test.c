/*
 * main_test.c
 *
 * Este arquivo cria o log do RnWriter, e, junto do RnWriter_test.c cria um script em lua
 * para fazer testes sobre o padrão implementado de leitores e escritores com
 * prioridade para escrita.
 *
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "safe.h"
#include "RnWriter.h"

#define N_WRITERS 2 // number of writer threads
#define N_READERS 2 // number of reader threads
#define DELAY 0.001  // fixed delay in seconds

pthread_mutex_t g_mutex;

void* writer_thread(void* args){
    RnWriter *rnwriter = (RnWriter*) args;

    while(1){
        double foo = 77777777777.7; // silly variable
        RnWriter_request_write(rnwriter);
        for (int i=0; i<9999999; i++) foo /= 2; // silly loop to simulate write time
        RnWriter_release_write(rnwriter);

        // sleep for (DELAY + random_between(0, 0.01)) seconds
        pthread_mutex_lock(&g_mutex);
        usleep(DELAY * 1000 + rand() % 10);
        pthread_mutex_unlock(&g_mutex);
    }

    pthread_exit(NULL);
}

void* reader_thread(void* args){
    RnWriter *rnwriter = (RnWriter*) args;

    while(1){
        double foo = 77777777777.7; // silly variable
        RnWriter_request_read(rnwriter);
        for (int i=0; i<9999999; i++) foo /= 2; // silly loop to simulate read time
        RnWriter_release_read(rnwriter);

        // sleep for (DELAY + random_between(0, 0.01)) seconds
        pthread_mutex_lock(&g_mutex);
        usleep(DELAY * 1000 + rand() % 10);
        pthread_mutex_unlock(&g_mutex);
    }

    pthread_exit(NULL);
}

int main(void){
    pthread_t writer_tids[N_WRITERS], reader_tids[N_READERS];
    pthread_mutex_init(&g_mutex, NULL);

    RnWriter* rnwriter = (RnWriter*) safe_malloc(sizeof(RnWriter));
    RnWriter_init(rnwriter);

    printf("require \"RnWriter\"\n\n");
    for(int i = 0; i < N_WRITERS; i++){
        pthread_create(&writer_tids[i], NULL, writer_thread, (void*) rnwriter);
    }
    for(int i = 0; i < N_READERS; i++){
        pthread_create(&reader_tids[i], NULL, reader_thread, (void*) rnwriter);
    }

    pthread_exit(NULL);
}
