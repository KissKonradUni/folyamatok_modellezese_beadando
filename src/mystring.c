#include "mystring.h"
#include "utils.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

implArray(string_view);

string* string_new(uint16_t cap) {
    assert(cap > 0);
    
    string* str;
    str = malloc(sizeof(string) + sizeof(char) * cap);
    assert(str != NULL);

    str->capacity = cap;
    str->length = 0;
    str->data = (typeof(str->data)) (str + 1);
    str->data[0] = '\0';
    return str;
}

void string_resize(string** str_ref, uint16_t new_cap) {
    assert(*str_ref != NULL);
    
    string* new_memory = malloc(sizeof(string) + sizeof(char) * new_cap);
    assert(new_memory != NULL);

    new_memory->capacity = new_cap;
    new_memory->length = (*str_ref)->length;
    new_memory->data = (typeof(new_memory->data)) (new_memory + 1);

    memcpy(new_memory->data, (*str_ref)->data, (*str_ref)->length);
    new_memory->data[new_memory->length] = '\0';
    free(*str_ref);
    *str_ref = new_memory;
}

void string_free(string* str) {
    free(str);
}

void string_append_char(string** str_ref, char c) {
    string* str = *str_ref;
    assert(str != NULL);

    if (str->capacity < str->length + 1) {
        string_resize(str_ref, str->capacity * 2);
        str = *str_ref;
    }
    str->data[str->length++] = c;
    str->data[str->length] = '\0';
}

void string_append_cstr(string** str_ref, char* cstring) {
    string* str = *str_ref;
    assert(str != NULL);

    uint32_t cstring_len = strlen(cstring);
    if (str->capacity < str->length + cstring_len) {
        string_resize(str_ref, str->length + cstring_len + 2);
        str = *str_ref;
    }
    memcpy(str->data + str->length, cstring, cstring_len);
    str->length += cstring_len;
    str->data[str->length] = '\0';
}

void string_append(string** str_ref, string* other) {
    assert(*str_ref != NULL);
    assert(other != NULL);

    string* str = *str_ref;
    if (str->capacity < str->length + other->length) {
        string_resize(str_ref, str->length + other->length + 2);
        str = *str_ref;
    }
    memcpy(str->data + str->length, other->data, other->length);
    str->length += other->length;
    str->data[str->length] = '\0';
}

void string_format(string** str_ref, char* format, ...) {
    va_list args;
    va_start(args, format);
    uint32_t min_capacity = vsnprintf(NULL, 0, format, args) + 1;
    va_end(args);

    uint32_t new_capacity = (*str_ref)->capacity;
    if (new_capacity < min_capacity) {
        new_capacity = min_capacity;
    }
    string_resize(str_ref, new_capacity);

    va_start(args, format);
    int result = vsnprintf((*str_ref)->data, (*str_ref)->capacity, format, args);
    va_end(args);

    assert_warn(result >= 0);
}

string_view* string_view_new(string* str, uint16_t start, uint16_t end) {
    assert(str != NULL);
    assert(start < end);
    assert(end <= str->length);

    string_view* view = malloc(sizeof(string_view));
    assert(view != NULL);

    view->length = end - start;
    view->data = str->data + start;
    return view;
}

void string_view_free(string_view* view) {
    free(view);
}

array(string_view)* string_split(string* str, char delim) {
    assert(str != NULL);

    uint16_t parts = 1;
    for (uint16_t i = 0; i < str->length; i++) {
        if (str->data[i] == delim) {
            parts++;
        }
    }

    array(string_view)* result = array_new(string_view, parts);
    uint16_t start = 0;
    uint16_t end = 0;
    uint16_t part = 0;
    for (uint16_t i = 0; i < str->length; i++) {
        if (str->data[i] == delim) {
            end = i;
            result->data[part].data = (str->data) + start;
            result->data[part].length = end - start;
            part++;
            start = i + 1;
        }
    }
    end = str->length;
    result->data[part] = *string_view_new(str, start, end);
    return result;
}
