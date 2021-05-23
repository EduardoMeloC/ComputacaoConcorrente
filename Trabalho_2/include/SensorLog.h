/*
 * LogBuffer.h
 *
 * Este arquivo define uma estrutura usada para guardar dados obtidos pelos sensores
 *
 */

 /* Struct que o sensor escreve no buffer */
typedef struct SensorLog_t
{
    float temperature;
    int sensor_id;
    int read_id;
} SensorLog;


