#ifndef ARRAY_H
#define ARRAY_H

#include <stdint.h>
#include <string.h>

#include "utils.h" // IWYU pragma: keep

#define defArray(type) \
    typedef struct __array_##type { \
        uint32_t length; \
        type * data; \
    } array_##type; \
    \
    array_##type* array_new_##type(uint32_t len); \
    void array_resize_##type(array_##type** array_ref, uint32_t new_len);

#define implArray(type) \
array_##type* array_new_##type(uint32_t len) { \
    assert(len > 0); \
    array_##type* pointer = malloc(sizeof(array_##type) + sizeof(type) * len); \
    assert(pointer != NULL); \
    pointer->length = len; \
    pointer->data = (typeof(pointer->data)) (pointer + 1); \
    return pointer; \
} \
\
void array_resize_##type(array_##type** array_ref, uint32_t new_len) { \
    assert(array_ref != NULL); \
    \
    array_##type* new_memory = malloc(sizeof(array_##type) + sizeof(type) * new_len); \
    assert(new_memory != NULL); \
    \
    new_memory->length = new_len; \
    new_memory->data = (typeof(new_memory->data)) (new_memory + 1); \
    \
    memcpy(new_memory->data, (*array_ref)->data, (*array_ref)->length); \
    free(*array_ref); \
    *array_ref = new_memory; \
}

#define array(type)                  array_##type
#define array_new(type, len)         array_new_##type(len)
#define array_resize(type, arr, len) array_resize_##type(arr, len)
#define array_free(arr)              free(arr)

#define foreach(iterator, array) \
    for (uint32_t iterator = 0; iterator < array->length; iterator++)

#endif // ARRAY_H