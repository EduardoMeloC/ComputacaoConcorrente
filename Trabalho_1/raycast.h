#ifndef RAYCAST_H
#define RAYCAST_H

#include <stdbool.h>
#include <math.h>
#include "vec.h"
#include "constants.h"
#include "utils.h"

typedef struct{
    vec3f origin;
    vec3f direction;
}Ray;

typedef struct{
    vec3f pos;
    vec3f albedo;
    float radius;
}Sphere;

typedef struct{ 
    Sphere* hitObject;
    vec3f point;
    vec3f normal;
    float distance;
}RayHit;

// Point Light
typedef struct{
    vec3f pos;
    vec3f color;
    float intensity;
}Light;

typedef struct{
    Light* lights;
    Sphere* spheres;
}Scene;

typedef struct {
    Scene* iScene;
    vec2f iResolution;
    int iNsamples;
}ShaderInput;

bool raycast(Ray* ray, Sphere* sphere, RayHit* hit);

void mainImage( vec3f* fragColor, vec2f fragCoord, ShaderInput* data );

#endif
