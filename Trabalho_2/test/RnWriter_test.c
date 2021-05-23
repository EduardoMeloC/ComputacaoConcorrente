/*
 * RnWriter_test.c
 *
 * Este arquivo cria o log do RnWriter, e, junto do main_test.c cria um script em lua
 * para fazer testes sobre o padrão implementado de leitores e escritores com
 * prioridade para escrita.
 *
 */ 

#include "RnWriter.h"

/* Initializes synchronization variables */
void RnWriter_init(RnWriter *rnwriter)
{ 
    pthread_mutex_init(&rnwriter->_mutex, NULL);
    pthread_cond_init(&rnwriter->_cond_read, NULL);
    pthread_cond_init(&rnwriter->_cond_write, NULL);

    rnwriter->_nReading = 0;
    rnwriter->_nWriting = 0;
    rnwriter->_nRequestedWrite = 0;
}

/* Destroys synchronization variables */
void RnWriter_destroy(RnWriter *rnwriter)
{
    RnWriter *this = rnwriter; // Syntax Sugar

    pthread_mutex_destroy(&rnwriter->_mutex);
    pthread_cond_destroy(&rnwriter->_cond_read);
    pthread_cond_destroy(&rnwriter->_cond_write);
    free(this);
}

/* Begins Reading */
void RnWriter_request_read(RnWriter *rnwriter)
{
    RnWriter *this = rnwriter; // Syntax Sugar

    pthread_mutex_lock(&this->_mutex);
    while(this->_nWriting > 0 || this->_nRequestedWrite > 0){
        printf("RnWriter:reader_blocked()\n");
        pthread_cond_wait(&this->_cond_read, &this->_mutex);
    }
    this->_nReading++;
    printf("RnWriter:reader_reading()\n");
    pthread_mutex_unlock(&this->_mutex);
}

/* Finishes Reading */
void RnWriter_release_read(RnWriter *rnwriter)
{
    RnWriter *this = rnwriter; // Syntax Sugar

    pthread_mutex_lock(&this->_mutex);
    this->_nReading--;
    if(this->_nReading == 0){
        pthread_cond_signal(&this->_cond_write);
    }
    printf("RnWriter:reader_released()\n");
    pthread_mutex_unlock(&this->_mutex);
}

/* Begins Writing */
void RnWriter_request_write(RnWriter *rnwriter)
{
    RnWriter *this = rnwriter; // Syntax Sugar

    pthread_mutex_lock(&this->_mutex);
    this->_nRequestedWrite++;
    printf("RnWriter:writer_wantsToWrite()\n");
    while(this->_nReading > 0 || this->_nWriting > 0){
        printf("RnWriter:writer_blocked()\n");
        pthread_cond_wait(&this->_cond_write, &this->_mutex);
    }
    this->_nRequestedWrite--;
    this->_nWriting++;
    printf("RnWriter:writer_writing()\n");
    pthread_mutex_unlock(&this->_mutex);
}

/* Finishes Writing */
void RnWriter_release_write(RnWriter *rnwriter)
{
    RnWriter *this = rnwriter; // Syntax Sugar

    pthread_mutex_lock(&this->_mutex);
    this->_nWriting--;
    pthread_cond_signal(&this->_cond_write);
    pthread_cond_broadcast(&this->_cond_read);
    printf("RnWriter:writer_released()\n");
    pthread_mutex_unlock(&this->_mutex);
}

