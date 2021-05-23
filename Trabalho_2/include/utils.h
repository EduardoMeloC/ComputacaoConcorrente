/*
 * utils.h
 *
 * Este arquivo define funções miscel0neas utilizadas na aplicação
 *
 */ 

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

#define COLOR_NORMAL "\x1B[0m"
#define COLOR_RED    "\x1B[31m"
#define COLOR_GREEN  "\x1B[32m"
#define COLOR_YELLOW "\x1B[33m"

/* Gera numero aleatorio thread-safe com rand_r */
float rand_between(float min, float max, unsigned int* state);

#endif
