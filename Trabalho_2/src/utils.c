/*
 * utils.c
 *
 * Este arquivo implementa funções miscelaneas utilizadas na aplicação
 *
 */

#include "utils.h"

/* Gera numero aleatorio thread-safe com rand_r */
float rand_between(float min, float max, unsigned int* state)
{
    float scale = rand_r(state) / (float) RAND_MAX; /* [0, 1.0] */
    return min + scale * (max - min); /* [min, max] */
}

