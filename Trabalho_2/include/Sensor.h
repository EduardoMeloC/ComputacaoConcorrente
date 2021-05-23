/*
 * Sensor.h
 *
 * Este arquivo define uma estrutura para o Sensor (que funciona como escritor)
 *
 */ 

#ifndef SENSOR_H 
#define SENSOR_H

#include <stdlib.h>
#include <unistd.h>
#include "RnWriter.h"
#include "LogBuffer.h"

/* Sensor funciona como escritor */
typedef struct Sensor_t
{
    RnWriter* rnwriter; // TAD que funciona como monitor
    LogBuffer* log_buffer; // Estrutura de dados compartilhada
    int id;
    unsigned int *rand_state;
} Sensor;

/* inicializa variáveis de um sensor previamente alocado */
void Sensor_init(Sensor* sensor, RnWriter* rnwriter, LogBuffer* log_buffer);

/* desaloca memória alocada internamente */
void Sensor_destroy(Sensor* sensor);

/* retorna temperatura lida pelo sensor */
int Sensor_get_temperature(Sensor* sensor);

/* escreve na estrutura compartilhada */
void Sensor_write(Sensor* sensor, float temperature);

/* função utilizada para gerar estado para geração de itens randomicos */
unsigned int get_rand_state();


/* thread executada pelo sensor */
void* Sensor_thread(void* sensor);

#endif
