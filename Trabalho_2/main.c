#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "safe.h"
#include "RnWriter.h"

#define COLOR_NORMAL "\x1B[0m"
#define COLOR_RED    "\x1B[31m"
#define COLOR_GREEN  "\x1B[32m"
#define COLOR_YELLOW "\x1B[33m"

/* Gera numero aleatorio thread-safe com rand_r */
float rand_between(float min, float max, unsigned int* state)
{
    float scale = rand_r(state) / (float) RAND_MAX; /* [0, 1.0] */
    return min + scale * (max - min); /* [min, max] */
}

/* Struct que o sensor escreve no buffer */
typedef struct SensorLog_t
{
    float temperature;
    int sensor_id;
    int read_id;
} SensorLog;

/* Struct utilizada para ser o buffer */
typedef struct LogBuffer_t
{
    int length;
    int write_index;
    SensorLog* buffer;
} LogBuffer;

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

/* Sensor funciona como escritor */
typedef struct Sensor_t
{
    RnWriter* rnwriter; // TAD que funciona como monitor
    LogBuffer* log_buffer; // Estrutura de dados compartilhada
    int id;
    unsigned int *rand_state;
} Sensor;

unsigned int get_rand_state(){
    /* https://stackoverflow.com/questions/43151361/how-to-create-thread-safe-random-number-generator-in-c-using-rand-r */
    return time(NULL) ^ getpid() ^ pthread_self();
}

int Sensor_get_temperature(Sensor* sensor){
    Sensor *this = sensor; // Syntax Sugar
    return rand_between(25, 40, this->rand_state);
}

void Sensor_write(Sensor* sensor, float temperature){
    Sensor *this = sensor; // Syntax Sugar

    // Cria o log e insere no buffer
    SensorLog sensorLog = { temperature, this->id, .read_id = 0 };
    sensor->log_buffer->buffer[sensor->log_buffer->write_index] = sensorLog;
    // Atualizar write_index
    sensor->log_buffer->write_index++;
    sensor->log_buffer->write_index %= sensor->log_buffer->length;
}

void* Sensor_thread(void* sensor)
{
    static int count = 0;
    Sensor *this = (Sensor *) sensor; // Syntax Sugar
    *this->rand_state = get_rand_state();

    while(count < 30){
        count++;
        // Medir Temperatura
        float temperature = Sensor_get_temperature(this);
        // Se a temperatura for maior do que 30, escreve no buffer
        if( temperature > 30 ){
            RnWriter_request_write(this->rnwriter);
            // Escreve no buffer
            Sensor_write(this, temperature); 
            RnWriter_release_write(this->rnwriter);
        }
        if(this->id == 1) LogBuffer_print(this->log_buffer);
        sleep(1);
    }
    pthread_exit(NULL);
}

/* Atuador funciona como leitor */
typedef struct Actuator_t
{
    RnWriter* rnwriter; // TAD que funciona como monitor
    LogBuffer* log_buffer; // Estrutura de dados compartilhada
    int id;
} Actuator;

void Actuator_danger(Actuator* actuator)
{
    Actuator* this = actuator; // Syntax Sugar
    const char* message = "!!! ALERTA VERMELHO !!!";
    printf("%sAtuador %d: %s%s\n", COLOR_NORMAL, this->id, COLOR_RED, message);
}

void Actuator_warn(Actuator* actuator)
{
    Actuator* this = actuator; // Syntax Sugar
    const char* message = "!! ALERTA AMARELO !!";
    printf("%sAtuador %d: %s%s\n", COLOR_NORMAL, this->id, COLOR_YELLOW, message);
}

void Actuator_safe(Actuator* actuator)
{
    Actuator* this = actuator; // Syntax Sugar
    const char* message = " Condição Normal ";
    printf("%sAtuador %d: %s%s\n", COLOR_NORMAL, this->id, COLOR_GREEN, message);
}

void Actuator_print_avgtemperature(Actuator* actuator, float avg)
{
    Actuator* this = actuator; // Syntax Sugar
    printf("%sAtuador %d: Temperatura média (%.1f°)\n", COLOR_NORMAL, this->id, avg);
}

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

void* Actuator_thread(void* actuator)
{
    static int count = 0;
    Actuator *this = (Actuator *) actuator; // Syntax Sugar

    while(count < 30){
        count++;
        RnWriter_request_read(this->rnwriter);
        Actuator_read(this);
        RnWriter_release_read(this->rnwriter);
        sleep(2);
    }
    pthread_exit(NULL);
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

void Sensor_init(Sensor* sensor, RnWriter* rnwriter, LogBuffer* log_buffer){
    static int id = 1;
    Sensor *this = sensor; // Syntax Sugar

    this->rand_state = (unsigned int*) safe_malloc(sizeof(unsigned int));
    this->rnwriter = rnwriter;
    this->log_buffer = log_buffer;
    this->id = id++;
}

void Sensor_destroy(Sensor* sensor){
    Sensor *this = sensor; // Syntax Sugar
    free(this->rand_state);
}

void Actuator_init(Actuator* actuator, RnWriter* rnwriter, LogBuffer* log_buffer){
    static int id = 1;
    Actuator *this = actuator; // Syntax Sugar

    this->rnwriter = rnwriter;
    this->log_buffer = log_buffer;
    this->id = id++;
}

void Actuator_destroy(Actuator* actuator){
    /* nothing to free */
}

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

    /* Esperando Threads Terminarem (para o programa rodar indefinidamente) */
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
