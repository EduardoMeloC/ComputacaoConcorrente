/*
 * Sensor.c
 *
 * Este arquivo implementa uma estrutura para o Sensor (que funciona como escritor)
 *
 */ 

#include "Sensor.h"

/* inicializa variáveis de um sensor previamente alocado */
void Sensor_init(Sensor* sensor, RnWriter* rnwriter, LogBuffer* log_buffer){
    static int id = 1;
    Sensor *this = sensor; // Syntax Sugar

    this->rand_state = (unsigned int*) safe_malloc(sizeof(unsigned int));
    this->rnwriter = rnwriter;
    this->log_buffer = log_buffer;
    this->id = id++;
}

/* desaloca memória alocada internamente */
void Sensor_destroy(Sensor* sensor){
    Sensor *this = sensor; // Syntax Sugar
    free(this->rand_state);
}

/* retorna temperatura lida pelo sensor */
int Sensor_get_temperature(Sensor* sensor){
    Sensor *this = sensor; // Syntax Sugar
    return rand_between(25, 40, this->rand_state);
}

/* escreve na estrutura compartilhada */
void Sensor_write(Sensor* sensor, float temperature){
    Sensor *this = sensor; // Syntax Sugar

    // Cria o log e insere no buffer
    SensorLog sensorLog = { temperature, this->id, .read_id = 0 };
    sensor->log_buffer->buffer[sensor->log_buffer->write_index] = sensorLog;
    // Atualizar write_index
    sensor->log_buffer->write_index++;
    sensor->log_buffer->write_index %= sensor->log_buffer->length;
}

/* função utilizada para gerar estado para geração de itens randomicos */
unsigned int get_rand_state(){
    /* https://stackoverflow.com/questions/43151361/how-to-create-thread-safe-random-number-generator-in-c-using-rand-r */
    return time(NULL) ^ getpid() ^ pthread_self();
}

/* thread executada pelo sensor */
void* Sensor_thread(void* sensor)
{
    Sensor *this = (Sensor *) sensor; // Syntax Sugar
    *this->rand_state = get_rand_state();

    while(1){
        // Medir Temperatura
        float temperature = Sensor_get_temperature(this);
        // Se a temperatura for maior do que 30, escreve no buffer
        if( temperature > 30 ){
            RnWriter_request_write(this->rnwriter);
            // Escreve no buffer
            Sensor_write(this, temperature); 
            RnWriter_release_write(this->rnwriter);
        }
        sleep(1);
    }
    pthread_exit(NULL);
}
