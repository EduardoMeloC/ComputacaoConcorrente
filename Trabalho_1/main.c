#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "safe.c"

#define DEFAULT_SCENE

#define MAX_DIST INFINITY // max ray distance

#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 720

#ifdef DEFAULT_SCENE
    // only mess around with these values if you change init_scene
    #define N_SPHERES 11 
    #define N_LIGHTS 3 
#endif

typedef enum { false, true } bool;

typedef struct{
    float x, y;
}vec2f;

typedef struct{
    float x, y, z;
}vec3f;

typedef struct{
    unsigned char r, g, b;
}color_t;

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
void vec3f_set(vec3f* u, float x, float y, float z){
    u->x = x;
    u->y = y;
    u->z = z;
}

vec3f vec3f_random(float min, float max){
    return (vec3f){ float_rand(min, max), float_rand(min, max), float_rand(min, max) };
}

float clamp(float v, float min, float max){
    return (v < min) ? min : (v > max) ? max : v;
}

vec3f vec3f_clamp(vec3f vec, float min, float max){
    return (vec3f){ clamp(vec.x, min, max), clamp(vec.y, min, max), clamp(vec.z, min, max) };
}

float dot(vec3f u, vec3f v){
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

vec3f sum(vec3f u, vec3f v){
    vec3f w = {u.x + v.x, u.y + v.y, u.z + v.z};
    return w;
}

vec3f sub(vec3f u, vec3f v){
    vec3f w = {u.x - v.x, u.y - v.y, u.z - v.z};
    return w;
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

float length(vec3f u){
    return sqrt(u.x*u.x + u.y*u.y + u.z*u.z);
}

vec3f normalize(vec3f u){
    float ulen = length(u);
    vec3f w = { u.x / ulen, u.y / ulen, u.z / ulen };
    return w;
}

float min(float a, float b){
    return (a < b) ? a : b;
}

float max(float a, float b){
    return (a > b) ? a : b;
}


color_t vec3f_to_color(vec3f c){
    color_t color = {
        (char)(255 * clamp(c.x, 0.0f, 1.0f)),
        (char)(255 * clamp(c.y, 0.0f, 1.0f)),
        (char)(255 * clamp(c.z, 0.0f, 1.0f))
    };
    return color;
}

bool raycast(Ray* ray, Sphere* sphere, RayHit* hit){ 
    // intersection test between sphere and ray
    // hit's value is updated to contain useful data
    // https://www.youtube.com/watch?v=HFPlKQGChpE
    vec3f ro = ray->origin;
    vec3f rd = ray->direction;
    vec3f s = sphere->pos;
    float r = sphere->radius;

    float t = dot(sub(s, ro), rd);
    if(t < 0) return false;
    vec3f p = sum(ro, mul(rd, t));

    float y = length(sub(s, p));
    if (y > r) return false;

    float x = sqrt(r*r - y*y);
    float t1 = t-x;
    float t2 = t+x;
    if (t1 > t2) swap(&t1, &t2);
    if (t1 < 0) {
        t1 = t2;
        if (t1 < 0) return false;
    }

    // ray hits sphere; update RayHit and return true
    hit->distance = t1;
    hit->point = sum(ray->origin, mul(ray->direction, hit->distance));
    hit->normal = normalize(sub(hit->point, sphere->pos));
    hit->hitObject = sphere;
    return true;
}

typedef struct {
    Scene* iScene;
    vec2f iResolution;
}ShaderInput;

// mainImage function was designed to be like a shader
void mainImage( vec3f* fragColor, vec2f fragCoord, ShaderInput* data ){
    static RayHit rayhit_infinity = { (Sphere*)NULL, (vec3f){0., 0., 0.}, (vec3f){0., 0., 0.}, MAX_DIST };
    // renaming shdader input for easier handling
    Scene* iScene = data->iScene;
    vec2f iResolution = data->iResolution;

    // centering uv coordinates
    vec2f uv = vec2f_div(vec2f_sub(fragCoord, vec2f_div(iResolution, 2.0f)), iResolution.y);
    vec3f color = { 0.0f, 0.0f, 0.0f };

    // create a ray for the current pixel
    vec3f ro = { 0.0f, 0.0f, -100.0f };
    vec3f rd = { uv.x, uv.y, 1.0f };
    rd = normalize(rd);
    Ray ray = { ro, rd };

    RayHit hit = rayhit_infinity;
    // iterate for each object in the scene
    for (int i = 0; i < N_SPHERES; i++){
        // check if ray intersects with object
        RayHit closestHit;
        bool is_hit = raycast(&ray, &iScene->spheres[i], &closestHit);
        if( is_hit ){
            if(closestHit.distance < hit.distance){
                hit = closestHit;
            }
        }
    }
    Sphere* hitObject = hit.hitObject;

    // render a sphere in the point light's position
    for (int i = 0; i < N_LIGHTS; i++){
        Light light = iScene->lights[i];
        RayHit hit;
        bool is_hit = raycast(&ray, &(Sphere){ light.pos, light.color, 0.5 }, &hit);
        if(is_hit){
            (*fragColor) = light.color;
            return;
        }
    }

    // if there was no hit, return background color
    if (hit.distance == MAX_DIST){
        (*fragColor) = (vec3f){ 0., 0., 0. };
        return;
    }

    // calculate diffuse
    for (int i=0; i < N_LIGHTS; i++){
        Light light = iScene->lights[i];
        vec3f lightDir = sub(light.pos, hit.point);
        float r2 = length(lightDir);
        lightDir = normalize(lightDir);
        float lightIntensity = light.intensity / (4 * M_PI * r2);
        float light_value = max(dot(lightDir, hit.normal), 0.f);

        color.x += hitObject->albedo.x * light.color.x * light_value * lightIntensity / M_PI;
        color.y += hitObject->albedo.y * light.color.y * light_value * lightIntensity / M_PI;
        color.z += hitObject->albedo.z * light.color.z * light_value * lightIntensity / M_PI;
        if(color.z > 1.0) printf("%.2f\n", color.z);
    }
    /* color = hit.hitObject->albedo; */
        

    (*fragColor) = vec3f_clamp(color, 0.0f, 1.0f);
}

Scene* init_scene(){
#ifdef DEFAULT_SCENE
    Light* lights = (Light*) safe_malloc(N_LIGHTS * sizeof(Light));
    lights[0].pos       = (vec3f){ -20., 20., -50. };
    lights[0].color     = (vec3f){ 1., 0., 1. };
    lights[0].intensity = 430.;
    lights[1].pos       = (vec3f){ 20., 20., -50. };
    lights[1].color     = (vec3f){ 0., 1., 1. };
    lights[1].intensity = 430.;
    lights[2].pos       = (vec3f){ 0., 20., -20*sqrt(3) };
    lights[2].color     = (vec3f){ 1., 1., 0.8 };
    lights[2].intensity = 210.;

    Sphere* spheres = (Sphere*) safe_malloc(N_SPHERES * sizeof(Sphere));
    for(int i=0; i < 10; i++){
        spheres[i].pos = (vec3f){ 30 * cos(M_PI / 10 * i), 0, 30 * sin(M_PI / 10 * i) - 45 };
        spheres[i].albedo = vec3f_random(0., 1.);
        spheres[i].radius = 5;
    }
    // add ground (sorry earth is not flat)
    spheres[10].radius = 20000;
    spheres[10].albedo = (vec3f){1., 1., 1.};
    spheres[10].pos = (vec3f){ spheres[5].pos.x, -spheres[10].radius - spheres[5].radius, spheres[5].pos.z };

    Scene* scene = (Scene*) safe_malloc(sizeof(Scene));
    scene->lights = lights;
    scene->spheres = spheres;
    return scene;
#endif
}

void free_scene(Scene* scene){
    free(scene->lights);
    free(scene->spheres);
    free(scene);
}

int main(int argc, char* argv[]){
    /* Intializes random number generator */
    srand((unsigned int)time(NULL));

    // allocate memory and initialize variables
    vec3f* frame_buffer = (vec3f*) safe_malloc(IMAGE_WIDTH * IMAGE_HEIGHT * sizeof(vec3f));
    Scene* scene = init_scene();
    vec2f resolution = { IMAGE_WIDTH, IMAGE_HEIGHT };
    ShaderInput shader_input = { scene, resolution };

    // iterate for each pixel
    for (int y=0; y < IMAGE_HEIGHT; y++){
        for (int x=0; x < IMAGE_WIDTH; x++){
            vec2f fragCoord = { x, IMAGE_HEIGHT-y };
            mainImage( &frame_buffer[y * IMAGE_WIDTH + x], fragCoord, &shader_input );
        }
    }
    
    // output frame_buffer
    FILE *output_image;
    output_image = fopen("output_image.ppm", "w");
    if (output_image == NULL){
        fprintf(stderr, "ERROR: couldn't open 'output_image.ppm'\n");
        exit(EXIT_FAILURE);
    }
    fprintf(output_image, "P6\n");
    fprintf(output_image, "%d %d 255\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    for (int y=0; y < IMAGE_HEIGHT; y++){
        for (int x=0; x < IMAGE_WIDTH; x++){
            vec3f pixel = frame_buffer[y * IMAGE_WIDTH + x];
            color_t c = vec3f_to_color(pixel);
            fputc(c.r, output_image);
            fputc(c.g, output_image);
            fputc(c.b, output_image);
        }    
    }
    fclose(output_image);
    free(frame_buffer);
    free_scene(scene);
    exit(EXIT_SUCCESS);
}
