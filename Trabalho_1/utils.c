#include <stdlib.h>

float float_rand( float min, float max )
{
    float scale = rand() / (float) RAND_MAX; /* [0, 1.0] */
    return min + scale * ( max - min );      /* [min, max] */
}

void swap( float* a, float* b ){
    float* c = a;
    (*a) = (*b);
    (*b) = (*c);
}

float clamp(float v, float min, float max){
    return (v < min) ? min : (v > max) ? max : v;
}

float min(float a, float b){
    return (a < b) ? a : b;
}

float max(float a, float b){
    return (a > b) ? a : b;
}
