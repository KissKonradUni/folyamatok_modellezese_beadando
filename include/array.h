#ifndef ARRAY_H
#define ARRAY_H

#include <stdint.h>

#include "utils.h" // IWYU pragma: keep

typedef void*(array_allocator_fn)(uint32_t len);

#define defArray(type) \
    typedef struct __array_##type { \
        uint32_t length; \
        type * data; \
    } array_##type; \
    \
    void* array_allocator_##type(uint32_t len); \

#define alloc_array(type, len) \
    malloc(sizeof(array_##type) + sizeof(type) * len)

#define implArray(type) \
    void* array_allocator_##type(uint32_t len) { \
        assert(sizeof(type) * len < UINT32_MAX); \
        return alloc_array(type, len); \
    } \

typedef struct __array {
    uint32_t length;
    void* data;
} array;

array* array_new_base   (array_allocator_fn allocator, uint32_t len);
void   array_resize_base(array_allocator_fn allocator, array** array_ref, uint32_t new_len);

#define array(type)                  array_##type
#define array_new(type, len)         (array_##type*) array_new_base(array_allocator_##type, len)
#define array_resize(type, arr, len) array_resize_base(array_allocator_##type, (array**)arr, len)
#define array_free(arr)              free(arr)

#define foreach(iterator, array) \
    for (uint32_t iterator = 0; iterator < array->length; iterator++)

#endif // ARRAY_H