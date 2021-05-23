/*
 * Actuator.h
 *
 * Este arquivo define uma estrutura para o Atuador (que funciona como leitor)
 *
 */ 

#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <stdlib.h>
#include <unistd.h>
#include "RnWriter.h"
#include "LogBuffer.h"

/* Atuador funciona como leitor */
typedef struct Actuator_t
{
    RnWriter* rnwriter; // TAD que funciona como monitor
    LogBuffer* log_buffer; // Estrutura de dados compartilhada
    int id;
} Actuator;

/* inicializa variáveis de um atuador previamente alocado */
void Actuator_init(Actuator* actuator, RnWriter* rnwriter, LogBuffer* log_buffer);

/* desaloca memória alocada internamente */
void Actuator_destroy(Actuator* actuator);

/* imprime mensagem de perigo (alerta vermelho) */
void Actuator_danger(Actuator* actuator);

/* imprime mensagem de cuidado (alerta amarelo) */
void Actuator_warn(Actuator* actuator);

/* imprime mensagem de condição normal (sinal verde) */
void Actuator_safe(Actuator* actuator);

/* imprime temperatura média */
void Actuator_print_avgtemperature(Actuator* actuator, float avg);

/* lê dados da estrutura compartilhada */
void Actuator_read(Actuator* actuator);

/* thread executada pelo atuador */
void* Actuator_thread(void* actuator);

#endif
