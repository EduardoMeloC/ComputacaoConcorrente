#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "safe.h"
#include "RnWriter.h"
#include "LogBuffer.h"
#include "Sensor.h"
#include "Actuator.h"

int main(int argc, char* argv[])
{
    /* Tratando Argumentos de Entrada */
    if(argc < 2){
        printf("Usage: %s <N_SENSORS> <(optional) BUFFER_LENGTH>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int nsensors = atoi(argv[1]);
    int bufflen = (argc == 2) ? 60 : atoi(argv[2]);

    /* Alocando Memória e Inicializando Estruturas */
    pthread_t sensor_tids[nsensors], actuator_tids[nsensors];
    Sensor sensors[nsensors]; Actuator actuators[nsensors];
    RnWriter* rnwriter;
    LogBuffer* log_buffer;

    rnwriter = (RnWriter*) safe_malloc(sizeof(RnWriter));
    RnWriter_init(rnwriter);
    log_buffer = (LogBuffer*) safe_malloc(sizeof(LogBuffer));
    LogBuffer_init(log_buffer, bufflen);
    for(int i = 0; i < nsensors; i++){
        Sensor_init(&sensors[i], rnwriter, log_buffer);
        Actuator_init(&actuators[i], rnwriter, log_buffer);
    }

    /* Criando Threads */
    for(int i = 0; i < nsensors; i++){
        safe_pthread_create(&sensor_tids[i], NULL, Sensor_thread, (void *)&sensors[i]);
        safe_pthread_create(&actuator_tids[i], NULL, Actuator_thread, (void*)&actuators[i]);
    }

    /* Imprime o LofBuffer na saída de 1 em 1 segundo */
#ifdef VERBOSE
    while(1){
        LogBuffer_print(log_buffer);
        sleep(1);
    }
#endif

    /* Esperando Threads Terminarem (do jeito que está, o programa roda indefinidamente) */
    for(int i = 0; i < nsensors; i++){
        safe_pthread_join(sensor_tids[i], NULL);
        safe_pthread_join(actuator_tids[i], NULL);
    }

    /* Liberando Recursos */
    RnWriter_destroy(rnwriter);
    LogBuffer_destroy(log_buffer);
    for(int i = 0; i < nsensors; i++){
        Sensor_destroy(&sensors[i]);
        Actuator_destroy(&actuators[i]);
    }

    exit(EXIT_SUCCESS);
}
