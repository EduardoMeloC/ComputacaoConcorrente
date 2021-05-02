#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* safe_malloc(size_t size){
    void *p = malloc(size);
    if (p == NULL){
        fprintf(stderr, "ERROR: failed to allocate %zu bytes.\n", size);
        exit(EXIT_FAILURE);
    }
    return p;
}

void* safe_calloc(size_t nmemb, size_t size){
    void *p = calloc(nmemb, size);
    if(p == NULL) {
        fprintf(stderr, "ERROR: failed to allocate %zu bytes.\n", size);
        exit(EXIT_FAILURE);
    }
    return p;
}

void safe_pthread_create(pthread_t *restrict thread,
                        const pthread_attr_t *restrict attr,
                        void *(*start_routine)(void *),
                        void *restrict arg)
{
    int is_successful = !pthread_create(thread, attr, start_routine, arg);
    if (!is_successful) {
        fprintf(stderr, "ERROR: failed to create thread on pthread_create.\n");
        exit(EXIT_FAILURE);
    }
}

void safe_pthread_join(pthread_t thread, void **retval){
    int is_successful = !pthread_join(thread, retval);
    if (!is_successful) {
        fprintf(stderr, "ERROR: failed to join thread on pthread_join.\n");
    }
}
