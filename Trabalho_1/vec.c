#include "vec.h"
#include "utils.h"

#include <math.h>

vec3f sum(vec3f u, vec3f v){
    vec3f w = {u.x + v.x, u.y + v.y, u.z + v.z};
    return w;
}

vec3f sub(vec3f u, vec3f v){
    vec3f w = {u.x - v.x, u.y - v.y, u.z - v.z};
    return w;
}

float  dot(vec3f u, vec3f v){
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

vec2f vec2f_sub(vec2f u, vec2f v){
    vec2f w = {u.x - v.x, u.y - v.y};
    return w;
}

vec3f mul(vec3f u, float value){
    vec3f w = {u.x * value, u.y * value, u.z * value};
    return w;
}

vec3f divide(vec3f u, float value){
    vec3f w = {(float)u.x / value, (float)u.y / value, (float)u.z / value};
    return w;
}

vec2f vec2f_div(vec2f u, float value){
    vec2f w = {(float)u.x / value, (float)u.y / value};
    return w;
}

vec2f vec2f_div_vec2f(vec2f u, vec2f v){
    vec2f w = {u.x/v.x, u.y/v.y};
    return w;
}

void vec3f_set(vec3f* u, float x, float y, float z){
    u->x = x;
    u->y = y;
    u->z = z;
}

vec3f vec3f_clamp(vec3f vec, float min, float max){
    return (vec3f){ clamp(vec.x, min, max), clamp(vec.y, min, max), clamp(vec.z, min, max) };
}

vec3f vec3f_random(float min, float max){
    return (vec3f){ float_rand(min, max), float_rand(min, max), float_rand(min, max) };
}

vec3f vec3f_reflect(vec3f dir, vec3f normal){
    return sub(mul(normal, dot(dir, normal)*2.f), dir);;
}

vec3f vec3f_mul_vec3f(vec3f u, vec3f v){
    return (vec3f) {u.x * v.x, u.y * v.y, u.z * v.z};
}


float length(vec3f u){
    return sqrt(u.x*u.x + u.y*u.y + u.z*u.z);
}

vec3f normalize(vec3f u){
    float ulen = length(u);
    vec3f w = { u.x / ulen, u.y / ulen, u.z / ulen };
    return w;
}

color_t vec3f_to_color(vec3f c){
    color_t color = {
        (char)(255 * clamp(c.x, 0.0f, 1.0f)),
        (char)(255 * clamp(c.y, 0.0f, 1.0f)),
        (char)(255 * clamp(c.z, 0.0f, 1.0f))
    };
    return color;
}
