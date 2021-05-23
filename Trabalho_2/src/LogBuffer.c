/*
 * LogBuffer.c
 *
 * Este arquivo implementa uma estrutura para o buffer compartilhado entre sensores/atuadores
 *
 */ 

#include "LogBuffer.h"

/* Imprime o LogBuffer */
void LogBuffer_print(LogBuffer* log_buffer)
{
    SensorLog* buff = log_buffer->buffer;

    printf("%s{\n", COLOR_NORMAL);
    for(int i=0; i < log_buffer->length; i++){
        printf("[ temperature: %.1f, \tsensor_id: %d, \tread_id: %d ]\n",
                buff[i].temperature, buff[i].sensor_id, buff[i].read_id);;
    }
    printf("}\n\n");
}

void LogBuffer_init(LogBuffer* log_buffer, int bufflen){
    LogBuffer *this = log_buffer; // Syntax Sugar
    this->buffer = (SensorLog*) safe_malloc(bufflen * sizeof(SensorLog));
    this->length = bufflen;
}

void LogBuffer_destroy(LogBuffer* log_buffer){
    LogBuffer *this = log_buffer; // Syntax Sugar
    free(this->buffer);
    free(this);
}
