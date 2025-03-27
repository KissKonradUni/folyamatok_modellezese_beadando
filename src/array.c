#include "array.h"

#include <string.h>

array* array_new_base(array_allocator_fn allocator, uint32_t len) { 
    assert(len > 0);
    array* pointer = allocator(len);
    assert(pointer != NULL); 
    pointer->length = len; 
    pointer->data = (typeof(pointer->data)) (pointer + 1); 
    return pointer; 
} 

void array_resize_base(array_allocator_fn allocator, array** array_ref, uint32_t new_len) { 
    assert(array_ref != NULL); 
    
    array* new_memory = allocator(new_len);
    assert(new_memory != NULL); 
    
    new_memory->length = new_len; 
    new_memory->data = (typeof(new_memory->data)) (new_memory + 1); 
    
    memcpy(new_memory->data, (*array_ref)->data, (*array_ref)->length); 
    free(*array_ref); 
    *array_ref = new_memory; 
}