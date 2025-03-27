#ifndef STRING_H
#define STRING_H

#include <stdint.h>

#include "array.h"

typedef struct __string {
    uint16_t capacity;
    uint16_t length;
    char* data;
} string;

typedef struct __string_view {
    uint16_t length;
    char* data;
} string_view;

defArray(string_view);

string* string_new        (uint16_t cap                       );
void    string_resize     (string** str_ref, uint16_t new_cap );
void    string_free       (string*  str                       );
void    string_append_char(string** str_ref, char c           );
void    string_append_cstr(string** str_ref, char* cstring    );
void    string_append     (string** str_ref, string* other    );
void    string_format     (string** str_ref, char* format, ...);

string_view*        string_view_new (string* str      , uint16_t start, uint16_t end);
void                string_view_free(string_view* view                              );
array(string_view)* string_split    (string* str      , char delim                  );

#endif // STRING_H