#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "safe.h"
#include "timer.h"
#include "vec.h"
#include "raycast.h"
#include "constants.h"
#include "utils.h"

// global shared memory between threads
ShaderInput g_shaderInput;
vec3f* g_frameBuffer;

// global input
int g_nthreads;

// function prototypes
Scene* initScene();
void free_scene(Scene* scene);

// function executed by thread
void* thread_fill_framebuffer(void* args){
    long int id = (long int) args;
    
    // iterate through the framebuffer calling the render function
    for (long int i = id; i < IMAGE_WIDTH * IMAGE_HEIGHT; i += g_nthreads){
        int y = i / IMAGE_WIDTH;
        int x = i - (y * IMAGE_WIDTH);
        vec2f fragCoord = { x, IMAGE_HEIGHT - y };
        mainImage( g_frameBuffer+i, fragCoord, &g_shaderInput );
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    // Declare timing variables
    double timer_start, timer_end;
    double fill_dt = 0, write_dt = 0, others_dt = 0;

    GET_TIME(timer_start);

    // Receive argv inputs 
    if (argc < 2){
        fprintf(stderr, "Usage: %s [N_THREADS] [N_SAMPLES]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    g_nthreads = atoi(argv[1]);
    int nsamples = atoi(argv[2]);

    // Intialize random number generator
    srand((unsigned int)time(NULL));

    // Allocate memory and initialize variables
    Scene* scene = initScene();
    vec2f resolution = { IMAGE_WIDTH, IMAGE_HEIGHT };
    g_shaderInput = (ShaderInput){ scene, resolution, nsamples };
    g_frameBuffer = (vec3f*) safe_malloc(IMAGE_WIDTH * IMAGE_HEIGHT * sizeof(vec3f));

    GET_TIME(timer_end);
    others_dt += timer_end - timer_start;
    GET_TIME(timer_start);

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

    GET_TIME(timer_end);
    fill_dt += timer_end - timer_start;
    GET_TIME(timer_start);
    
    // output frame_buffer to a file
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

    GET_TIME(timer_end);
    write_dt += timer_end - timer_start;
    GET_TIME(timer_start);
    
    // free resources
    fclose(output_image);
    free_scene(scene);
    free(g_frameBuffer);

    GET_TIME(timer_end);
    others_dt += timer_end - timer_start;

    // output execution time:
    printf("fill execution time:\t %.2fs\n", fill_dt);
    printf("write execution time:\t %.2fs\n", write_dt);
    printf("others execution time:\t %.2fs\n", others_dt);
    printf("total execution time:\t %.2fs\n", fill_dt + write_dt + others_dt);;

    exit(EXIT_SUCCESS);
}

// create spheres and lights
Scene* initScene(){
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
    // add ground (which isn't flat, by the way)
    spheres[10].radius = 20000;
    spheres[10].albedo = (vec3f){1., 1., 1.};
    spheres[10].pos = (vec3f){ spheres[5].pos.x, -spheres[10].radius - spheres[5].radius, spheres[5].pos.z };

    Scene* scene = (Scene*) safe_malloc(sizeof(Scene));
    scene->lights = lights;
    scene->spheres = spheres;
    return scene;
}

// free scene resources
void free_scene(Scene* scene){
    free(scene->lights);
    free(scene->spheres);
    free(scene);
}
