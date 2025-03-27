#ifndef STRING_H
#define STRING_H

#include <stdint.h>

#include "array.h"

/**
 * @brief Read only string structure
 */
typedef struct __string_view {
    uint16_t length;
    char* c_str;
} string_view;

/**
 * @brief Dynamic string structure
 */
typedef struct __string {
    bool local;
    uint16_t capacity;
    union {
        struct {
            uint16_t length;
            char* c_str;
        };
        string_view as_view;
    };
} string;

defArray(string_view);

string* string_new        (uint16_t cap                             );
void    string_resize     (string** str_ref, uint16_t new_cap       );
void    string_free       (string*  str                             );
void    string_append_char(string** str_ref, char c                 );
void    string_append_cstr(string** str_ref, const char* cstring    );
void    string_append_view(string** str_ref, const string_view* view);
void    string_format     (string** str_ref, const char* format, ...);
string* string_clone      (string* str                              );

/**
 * @brief Quick and dirty alias for creating a read only string primitive
 */
#define const_str(value) \
    (string_view) { \
        .length = sizeof(value) - 1, \
        .c_str = value \
    }

string_view*        string_view_new        (const string* str      , uint16_t start, uint16_t end);
void                string_view_free       (      string_view* view                              );
bool                string_view_starts_with(const string_view* str , const string_view* start    );
bool                string_view_ends_with  (const string_view* str , const string_view* end      );
bool                string_view_equals     (const string_view* str , const string_view* other    );
array(string_view)* string_view_split      (const string_view* str , char delim                  );

#endif // STRING_H