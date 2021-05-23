/*
 * RnWriter.h
 *
 * Este arquivo define uma interface para o problema de Leitores e
 * Escritores com prioridade para escrita.
 *
 */ 

#ifndef RNWRITER_H
#define RNWRITER_H

#include <pthread.h>
#include "safe.h"

typedef struct RnWriter_t
{
    // Synchronization variables
    pthread_mutex_t _mutex;
    pthread_cond_t _cond_read, _cond_write;

    unsigned int _nReading;        // number of threads reading data 
    unsigned int _nWriting;        // number of threads writing data 
    unsigned int _nRequestedWrite; // number of threads that requested to write
} RnWriter;

/* Initializes synchronization variables */
void RnWriter_init(RnWriter *rnwriter);

/* Destroys synchronization variables */
void RnWriter_destroy(RnWriter *rnwriter);

/* Begins Reading */
void RnWriter_request_read(RnWriter *rnwriter);

/* Finishes Reading */
void RnWriter_release_read(RnWriter *rnwriter);

/* Begins Writing */
void RnWriter_request_write(RnWriter *rnwriter);

/* Finishes Writing */
void RnWriter_release_write(RnWriter *rnwriter);

#endif
