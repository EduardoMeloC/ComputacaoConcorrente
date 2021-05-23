/*
 * LogBuffer.h
 *
 * Este arquivo define uma estrutura para o buffer compartilhado entre sensores/atuadores
 *
 */ 

#ifndef LOGBUFFER_H 
#define LOGBUFFER_H 

#include <stdio.h>
#include "safe.h"
#include "utils.h"
#include "SensorLog.h"

/* Struct utilizada para ser o buffer */
typedef struct LogBuffer_t
{
    int length;
    int write_index;
    SensorLog* buffer;
} LogBuffer;

/* Imprime o LogBuffer */
void LogBuffer_print(LogBuffer* log_buffer);

/* Atribui variáveis de um log_buffer previamente alocado */
void LogBuffer_init(LogBuffer* log_buffer, int bufflen);

/* Desaloca memória alocada internamente */
void LogBuffer_destroy(LogBuffer* log_buffer);
#endif
