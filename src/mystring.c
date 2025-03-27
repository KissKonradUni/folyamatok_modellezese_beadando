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

    str->local = false;
    str->capacity = cap;
    str->length = 0;
    str->c_str = (typeof(str->c_str)) (str + 1);
    str->c_str[0] = '\0';
    return str;
}

void string_resize(string** str_ref, uint16_t new_cap) {
    assert(*str_ref != NULL);
    
    string* new_memory = malloc(sizeof(string) + sizeof(char) * new_cap);
    assert(new_memory != NULL);

    new_memory->capacity = new_cap;
    new_memory->length = (*str_ref)->length;
    new_memory->c_str = (typeof(new_memory->c_str)) (new_memory + 1);

    memcpy(new_memory->c_str, (*str_ref)->c_str, (*str_ref)->length);
    new_memory->c_str[new_memory->length] = '\0';
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
    str->c_str[str->length++] = c;
    str->c_str[str->length] = '\0';
}

void string_append_cstr(string** str_ref, char* cstring) {
    string* str = *str_ref;
    assert(str != NULL);

    uint32_t cstring_len = strlen(cstring);
    if (str->capacity < str->length + cstring_len) {
        string_resize(str_ref, str->length + cstring_len + 2);
        str = *str_ref;
    }
    memcpy(str->c_str + str->length, cstring, cstring_len);
    str->length += cstring_len;
    str->c_str[str->length] = '\0';
}

void string_append(string** str_ref, string* other) {
    assert(*str_ref != NULL);
    assert(other != NULL);

    string* str = *str_ref;
    if (str->capacity < str->length + other->length) {
        string_resize(str_ref, str->length + other->length + 2);
        str = *str_ref;
    }
    memcpy(str->c_str + str->length, other->c_str, other->length);
    str->length += other->length;
    str->c_str[str->length] = '\0';
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
    int result = vsnprintf((*str_ref)->c_str, (*str_ref)->capacity, format, args);
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
    view->c_str = str->c_str + start;
    return view;
}

void string_view_free(string_view* view) {
    free(view);
}

bool string_view_starts_with(string_view* str, string_view* start) {
    assert(str != NULL);
    assert(start != NULL);

    return memcmp(str->c_str, start->c_str, start->length) == 0;
}

bool string_view_ends_with(string_view* str, string_view* end) {
    assert(str != NULL);
    assert(end != NULL);

    return memcmp(str->c_str + str->length - end->length, end->c_str, end->length) == 0;
}

bool string_view_equals(string_view* str, string_view* other) {
    assert(str != NULL);
    assert(other != NULL);

    return str->length == other->length && memcmp(str->c_str, other->c_str, str->length) == 0;
}

array(string_view)* string_view_split(string_view* str, char delim) {
    assert(str != NULL);

    uint16_t parts = 1;
    for (uint16_t i = 0; i < str->length; i++) {
        if (str->c_str[i] == delim) {
            parts++;
        }
    }

    array(string_view)* result = array_new(string_view, parts);
    uint16_t start = 0;
    uint16_t end = 0;
    uint16_t part = 0;
    for (uint16_t i = 0; i < str->length; i++) {
        if (str->c_str[i] == delim) {
            end = i;
            char* pointer = (str->c_str) + start;
            result->data[part].c_str = pointer;
            result->data[part].length = end - start;
            part++;
            start = i + 1;
        }
    }
    end = str->length;
    result->data[part] = (string_view) {end - start, str->c_str + start};
    return result;
}