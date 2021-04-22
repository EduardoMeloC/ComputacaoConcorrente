#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "safe.c"

#define DEFAULT_SCENE

#define MAX_DIST INFINITY // max ray distance

#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 720

#define SHADOW_BIAS 1e-4f
#define MSAA_SAMPLES 16 // samples per pixel for anti-aliasing

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

float clamp(float v, float min, float max){
    return (v < min) ? min : (v > max) ? max : v;
}

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
    if (t1 > t2){ 
        swap(&t1, &t2);
    }
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
    vec2f pixel_size = (vec2f){ 2.f/iResolution.x, 1.f/iResolution.y };
    vec3f color = { 0.0f, 0.0f, 0.0f };

    // make samples per pixel for anti-aliasing with 2x2 grid
    vec3f MSAA_samples[MSAA_SAMPLES];
    float sq_samples = sqrt(MSAA_SAMPLES);
    vec2f subpix_len = (vec2f){ pixel_size.x / sq_samples, pixel_size.y / sq_samples };
    for(int sample = 0; sample < MSAA_SAMPLES; sample++){ 
        vec2f MSAA_offset = { 
            fmod(sample * subpix_len.x, pixel_size.x) + subpix_len.x/2,
            floor(sample * subpix_len.x / pixel_size.x) * subpix_len.y + subpix_len.y/2
        };
            /* pixel_size.x / sq_samples / 2. * sample + (pixel_size.x / sq_samples) / 4., */
            /* (pixel_size.x / sq_samples / 2. * sample + (pixel_size.x / sq_samples) / 4.) / pixel_size.x * pixel_size.y / 2. + (pixel_size.y / sq_samples) /4. }; */


        // create a ray for the current pixel
        vec3f ro = { 0.0f , 0.0f, -70.0f };
        vec3f rd = { uv.x + MSAA_offset.x, uv.y + MSAA_offset.y, 1.0f }; 
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
        bool is_light_hit;
        for (int i = 0; i < N_LIGHTS; i++){
            Light light = iScene->lights[i];
            RayHit hit;
            is_light_hit = raycast(&ray, &(Sphere){ light.pos, light.color, 0.5 }, &hit);
            if (is_light_hit) {
                color = light.color;
                break;
            }
        }
        if(is_light_hit){
            MSAA_samples[sample] = color;
            continue;
        }

        // if there was no hit, return background color
        if (hit.distance == MAX_DIST){
            MSAA_samples[sample] = (vec3f){ 0., 0., 0. };
            continue;
        }

        vec3f diffuse  = (vec3f){0., 0., 0.};
        vec3f specular = (vec3f){0., 0., 0.};
        for (int i=0; i < N_LIGHTS; i++){
            Light light = iScene->lights[i];
            vec3f light_dir = sub(light.pos, hit.point);
            float r2 = length(light_dir);
            light_dir = normalize(light_dir);

            // cast hard shadow
            float shadow_value = 1.;
            vec3f shadow_ro = sum(hit.point, mul(hit.normal, SHADOW_BIAS));
            vec3f shadow_rd = mul(light_dir, 1);
            Ray shadow_ray = (Ray) {shadow_ro, shadow_rd};
            RayHit shadow_hit;
            for (int i = 0; i < N_SPHERES-1; i++){
                bool is_hit = raycast(&shadow_ray, &iScene->spheres[i], &shadow_hit);
                if(shadow_hit.distance > length(sub(light.pos, shadow_ray.origin))) continue;
                if (is_hit){
                    shadow_value = 0.;
                }
            }

            // calculate diffuse
            vec3f light_intensity = mul(light.color, light.intensity / (4 * M_PI * r2)); // controlling decay
            float light_value = max(dot(light_dir, hit.normal), 0.f);
            diffuse = sum(diffuse, mul(vec3f_mul_vec3f(hitObject->albedo, light_intensity), light_value * shadow_value ));

            // calculate specular (Phong)
            vec3f R = vec3f_reflect(light_dir, hit.normal);
            specular = sum(specular, mul(light_intensity, shadow_value * pow(max(0.f, dot(R, mul(rd, -1))), 150)));


        }
        MSAA_samples[sample] = sum(mul(diffuse, 0.8f), mul(specular, 0.3f));
    }

    for(int i=0; i < MSAA_SAMPLES; i++){
        color = sum(color, MSAA_samples[i]);
    }
    color = mul(color, 1./MSAA_SAMPLES);
        

    (*fragColor) = vec3f_clamp(color, 0.0f, 1.0f);
}

Scene* init_scene(){
#ifdef DEFAULT_SCENE
    Light* lights = (Light*) safe_malloc(N_LIGHTS * sizeof(Light));
    lights[0].pos       = (vec3f){ -20., 20., -50. };
    lights[0].color     = (vec3f){ 1., 0., 1. };
    lights[0].intensity = 135.;
    lights[1].pos       = (vec3f){ 20., 20., -50. };
    lights[1].color     = (vec3f){ 0., 1., 1. };
    lights[1].intensity = 135.;
    lights[2].pos       = (vec3f){ 0., 20., -20*sqrt(3) };
    lights[2].color     = (vec3f){ 1., 1., 0.8 };
    lights[2].intensity = 165.;

    Sphere* spheres = (Sphere*) safe_malloc(N_SPHERES * sizeof(Sphere));
    for(int i=1; i < 10; i++){
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

// global inputs
int g_nthreads;
int g_nsamples;

// global shared memory between threads
ShaderInput g_shaderInput;
vec3f* g_frameBuffer;

void* thread_fill_framebuffer(void* args){
    long int id = (long int) args;
    
    for (long int i = id; i < IMAGE_WIDTH * IMAGE_HEIGHT; i += g_nthreads){
        int y = i / IMAGE_WIDTH;
        int x = i - (y * IMAGE_WIDTH);
        vec2f fragCoord = { x, IMAGE_HEIGHT - y };
        mainImage( g_frameBuffer+i, fragCoord, &g_shaderInput );
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    // Receive argv inputs 
    if (argc < 2){
        fprintf(stderr, "Usage: %s [N_THREADS] [N_SAMPLES]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    g_nthreads = atoi(argv[1]);
    g_nsamples = atoi(argv[2]);

    // Intialize random number generator
    srand((unsigned int)time(NULL));

    // Allocate memory and initialize variables
    Scene* scene = init_scene();
    vec2f resolution = { IMAGE_WIDTH, IMAGE_HEIGHT };
    g_shaderInput = (ShaderInput){ scene, resolution };
    g_frameBuffer = (vec3f*) safe_malloc(IMAGE_WIDTH * IMAGE_HEIGHT * sizeof(vec3f));

    // Sequential version
    if (g_nthreads == 0){
        for (int y=0; y < IMAGE_HEIGHT; y++){
            for (int x=0; x < IMAGE_WIDTH; x++){
                vec2f fragCoord = { x, IMAGE_HEIGHT-y };
                mainImage( &g_frameBuffer[y * IMAGE_WIDTH + x], fragCoord, &g_shaderInput );
            }
        }
    }
    // Multi threaded version
    else{
        pthread_t threads[g_nthreads];
        for (long int i = 0; i < g_nthreads; i++){
            safe_pthread_create(&threads[i], NULL, thread_fill_framebuffer, (void *) i);
        }
        for (int i = 0; i < g_nthreads; i++){
            safe_pthread_join(threads[i], NULL);
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
            vec3f pixel = g_frameBuffer[y * IMAGE_WIDTH + x];
            color_t c = vec3f_to_color(pixel);
            fputc(c.r, output_image);
            fputc(c.g, output_image);
            fputc(c.b, output_image);
        }    
    }
    fclose(output_image);
    free_scene(scene);
    free(g_frameBuffer);
    exit(EXIT_SUCCESS);
}
