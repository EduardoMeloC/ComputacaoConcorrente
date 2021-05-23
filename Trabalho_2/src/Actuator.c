/*
 * Actuator.c
 *
 * Este arquivo implementa uma estrutura para o Atuador (que funciona como leitor)
 *
 */ 

#include "Actuator.h"

/* inicializa variáveis de um atuador previamente alocado */
void Actuator_init(Actuator* actuator, RnWriter* rnwriter, LogBuffer* log_buffer){
    static int id = 1;
    Actuator *this = actuator; // Syntax Sugar

    this->rnwriter = rnwriter;
    this->log_buffer = log_buffer;
    this->id = id++;
}

/* desaloca memória alocada internamente */
void Actuator_destroy(Actuator* actuator){
    /* nothing to free */
}

/* imprime mensagem de perigo (alerta vermelho) */
void Actuator_danger(Actuator* actuator)
{
    Actuator* this = actuator; // Syntax Sugar
    const char* message = "!!! ALERTA VERMELHO !!!";
    printf("%sAtuador %d: %s%s\n", COLOR_NORMAL, this->id, COLOR_RED, message);
}

/* imprime mensagem de cuidado (alerta amarelo) */
void Actuator_warn(Actuator* actuator)
{
    Actuator* this = actuator; // Syntax Sugar
    const char* message = "!! ALERTA AMARELO !!";
    printf("%sAtuador %d: %s%s\n", COLOR_NORMAL, this->id, COLOR_YELLOW, message);
}

/* imprime mensagem de condição normal (sinal verde) */
void Actuator_safe(Actuator* actuator)
{
    Actuator* this = actuator; // Syntax Sugar
    const char* message = " Condição Normal ";
    printf("%sAtuador %d: %s%s\n", COLOR_NORMAL, this->id, COLOR_GREEN, message);
}

/* imprime temperatura média */
void Actuator_print_avgtemperature(Actuator* actuator, float avg)
{
    Actuator* this = actuator; // Syntax Sugar
    printf("%sAtuador %d: Temperatura média (%.1f°)\n", COLOR_NORMAL, this->id, avg);
}

/* lê dados da estrutura compartilhada */
void Actuator_read(Actuator* actuator)
{
    Actuator* this = actuator; // Syntax Sugar
    SensorLog* buffer = this->log_buffer->buffer;

    /* Variáveis Temporárias da Leitura */
    int last_temperatures[15]; // Vetor para ver as ultimas 15 temperaturas
    int alert_count = 0; // Usado para emitir alerta amarelo
    int danger_count = 0; // Usado para emitir alerta vermelho
    int n_temperatures = 0; // Contador de temperaturas lidas
    float total_temperature = 0; // Somatório das temperaturas

    // Itera pelo buffer lendo os dados escritos pelo sensor de mesmo id
    for(int i = 0; i < this->log_buffer->length; i++){
        if (buffer[i].sensor_id != this->id) continue;
        // decrementa valor de alerta se remover temperatura alta de last_temperatures
        if (n_temperatures >= 15 && last_temperatures[n_temperatures % 15] > 35){
            alert_count--;
        }
        // atualiza buffer de ultimas temperaturas e contador/somatorio de temperaturas
        last_temperatures[n_temperatures % 15] = buffer[i].temperature;
        n_temperatures++;
        total_temperature += buffer[i].temperature;
        buffer[i].read_id++;
        // atualiza variaveis de alerta se a temperatura for alta
        if (buffer[i].temperature > 35){
            alert_count++;
            danger_count++;
        }
        else{
            danger_count = 0;
        }
    }
    // Se existir condição de alerta, emita alerta
    if(danger_count >= 5){
        Actuator_danger(this);
    }
    else if(alert_count >= 5){
        Actuator_warn(this);
    }
    else{
        Actuator_safe(this);
    }
    if (n_temperatures > 0){
        Actuator_print_avgtemperature(this, total_temperature/n_temperatures);        
    }
}

/* thread executada pelo atuador */
void* Actuator_thread(void* actuator)
{
    Actuator *this = (Actuator *) actuator; // Syntax Sugar

    while(1){
        RnWriter_request_read(this->rnwriter);
        Actuator_read(this);
        RnWriter_release_read(this->rnwriter);
        sleep(2);
    }
    pthread_exit(NULL);
}
