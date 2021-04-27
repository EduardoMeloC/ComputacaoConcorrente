#ifndef VEC_H
#define VEC_H

typedef struct{
    float x, y;
}vec2f;

typedef struct{
    float x, y, z;
}vec3f;

typedef struct{
    unsigned char r, g, b;
}color_t;

vec3f sum(vec3f u, vec3f v);

vec3f sub(vec3f u, vec3f v);

float  dot(vec3f u, vec3f v);

vec2f vec2f_sub(vec2f u, vec2f v);

vec3f mul(vec3f u, float value);

vec3f divide(vec3f u, float value);

vec2f vec2f_div(vec2f u, float value);

vec2f vec2f_div_vec2f(vec2f u, vec2f v);

void vec3f_set(vec3f* u, float x, float y, float z);

vec3f vec3f_clamp(vec3f vec, float min, float max);

vec3f vec3f_random(float min, float max);

vec3f vec3f_reflect(vec3f dir, vec3f normal);

vec3f vec3f_mul_vec3f(vec3f u, vec3f v);

float length(vec3f u);

vec3f normalize(vec3f u);

color_t vec3f_to_color(vec3f c);

#endif
