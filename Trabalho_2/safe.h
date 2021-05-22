#ifndef SAFE_H
#define SAFE_H

#include <stdlib.h>

void* safe_malloc(size_t size);

void* safe_calloc(size_t nmemb, size_t size);

void safe_pthread_create(pthread_t *restrict thread,
                        const pthread_attr_t *restrict attr,
                        void *(*start_routine)(void *),
                        void *restrict arg);

void safe_pthread_join(pthread_t thread, void **retval);

#endif
