/*
 * RnWriter.c
 *
 * Este arquivo implementa uma interface concorrente para o problema
 * de Leitores e Escritores com prioridade para escrita.
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
    pthread_mutex_destroy(&rnwriter->_mutex);
    pthread_cond_destroy(&rnwriter->_cond_read);
    pthread_cond_destroy(&rnwriter->_cond_write);
}

/* Begins Reading */
void RnWriter_request_read(RnWriter *rnwriter)
{
    RnWriter *this = rnwriter; // Syntax Sugar

    pthread_mutex_lock(&this->_mutex);
    while(this->_nWriting > 0 || this->_nRequestedWrite > 0){
        pthread_cond_wait(&this->_cond_read, &this->_mutex);
    }
    this->_nReading++;
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
    pthread_mutex_unlock(&this->_mutex);
}

/* Begins Writing */
void RnWriter_request_write(RnWriter *rnwriter)
{
    RnWriter *this = rnwriter; // Syntax Sugar

    pthread_mutex_lock(&this->_mutex);
    this->_nRequestedWrite++;
    while(this->_nReading > 0 || this->_nWriting > 0){
        pthread_cond_wait(&this->_cond_write, &this->_mutex);
    }
    this->_nRequestedWrite--;
    this->_nWriting++;
    pthread_mutex_unlock(&this->_mutex);
}

/* Finishes Writing */
void RnWriter_release_write(RnWriter *rnwriter)
{
    RnWriter *this = rnwriter; // Syntax Sugar

    pthread_mutex_lock(&this->_mutex);;
    this->_nWriting--;
    pthread_cond_signal(&this->_cond_write);
    pthread_cond_broadcast(&this->_cond_read);
    pthread_mutex_unlock(&this->_mutex);
}

