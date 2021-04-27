#include "raycast.h"

#include "vec.h"
#include <stddef.h>

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

    // make samples per pixel for anti-aliasing with sqrt(samples) x sqrt(samples) grid
    vec3f MSAA_samples[MSAA_SAMPLES];
    float sq_samples = sqrt(MSAA_SAMPLES);
    vec2f subpix_len = (vec2f){ pixel_size.x / sq_samples, pixel_size.y / sq_samples };
    for(int sample = 0; sample < MSAA_SAMPLES; sample++){ 
        vec2f MSAA_offset = { 
            fmod(sample * subpix_len.x, pixel_size.x) + subpix_len.x/2,
            floor(sample * subpix_len.x / pixel_size.x) * subpix_len.y + subpix_len.y/2
        };

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


