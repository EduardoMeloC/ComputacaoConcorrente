#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 720

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
    vec3f point;
    vec3f normal;
    float distance;
}RayHit;

void vec3f_set(vec3f* u, float x, float y, float z){
    u->x = x;
    u->y = y;
    u->z = z;
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

vec2f v2sub(vec2f u, vec2f v){
    vec2f w = {u.x - v.x, u.y - v.y};
    return w;
}

vec3f mul(vec3f u, float value){
    vec3f w = {u.x * value, u.y * value, u.z * value};
    return w;
}

vec2f divd(vec2f u, float value){
    vec2f w = {(float)u.x / value, (float)u.y / value};
    return w;
}

vec2f vec_divd(vec2f u, vec2f v){
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

float clamp(float v, float min, float max){
    return (v < min) ? min : (v > max) ? max : v;
}

color_t vec3f_to_color(vec3f c){
    color_t color = {
        (char)(255 * clamp(c.x, 0.0f, 1.0f)),
        (char)(255 * clamp(c.y, 0.0f, 1.0f)),
        (char)(255 * clamp(c.z, 0.0f, 1.0f))
    };
    return color;
}

/* RayHit cast(Ray ray){ */
     
/* } */

void mainImage( vec3f* fragColor, vec2f fragCoord, vec2f iResolution ){
    // centering uv coordinates
    vec2f uv = divd(v2sub(fragCoord, divd(iResolution, 2.0f)), iResolution.y);
    vec3f color = { 0.0f, 0.0f, 0.0f };

    vec3f ro = { 0.0f, 0.0f, 0.0f };
    vec3f rd = { uv.x, uv.y, 1.0f };
    rd = normalize(rd);

    vec3f s = { 0.0f, 0.0f, 4.0f };
    float r = 1.0f;

    float t = dot(sub(s, ro), rd);
    vec3f p = sum(ro, mul(rd, t));

    float y = length(sub(s, p));
    if (y < r){
        float x = sqrt(r*r - y*y);
        float t1 = t-x;
        float t2 = t+x;

        vec3f_set(&color, t1, t1, t1);
    }

    (*fragColor) = color;
}

int main(int argc, char* argv[]){
    // allocate memory and initialize variables
    vec3f* frame_buffer = (vec3f*) malloc(IMAGE_WIDTH * IMAGE_HEIGHT * sizeof(vec3f));

    vec2f resolution = { IMAGE_WIDTH, IMAGE_HEIGHT };

    // iterate for each pixel
    for (int y=0; y < IMAGE_HEIGHT; y++){
        for (int x=0; x < IMAGE_WIDTH; x++){
            vec2f fragCoord = { x, IMAGE_HEIGHT-y };
            mainImage( &frame_buffer[y * IMAGE_WIDTH + x], fragCoord, resolution );

            /* vec3f ro = { 0.0f, 0.0f, 0.0f }; */
            /* vec3f rd = { uv.x, uv.y, 1.0f }; */
            /* rd = normalize(rd); */
            /* Ray ray = { ro, rd }; */

            /* RayHit hit = cast(ray); */
            /* frame_buffer[y * IMAGE_WIDTH + x] = fragColor; */
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
            /* printf("%f %f %f\n", c.r, c.g, c.b); */
            /* printf("index: %d\n", y * IMAGE_WIDTH + x); */
            /* printf("%u %u %u\n", r, g, b); */
        }    
    }
    fclose(output_image);
    free(frame_buffer);
    /* free(ray); */
    /* free(hit); */
    exit(EXIT_SUCCESS);
}
