#include "file.h"

#include <string.h>
#include <unistd.h>

string* get_cwd_str() {
    string* result = string_new(PATH_MAX);
    getcwd(result->c_str, result->capacity);
    result->length = strlen(result->c_str);
    return result;
}

string* read_all_lines(FILE* file) {
    assert(file != NULL);

    // Get the size of the file
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Create a string with the size of the file
    string* result = string_new(size + 1);

    // Read the file into the string
    ON_WINDOWS(int lineIndex = 1;); // This variable is only used on Windows
    while (fgets((result->c_str) + (result->length), size - result->length, file)) {
        // The linecounting is used to emit the \0 terminators
        // that fgets adds to the end of every line.
        // This does mean that we allocate an extra byte for every line.
        ON_WINDOWS(
            result->length = ftell(file) - (lineIndex++);
        );
        ON_UNIX(
            result->length = ftell(file);
            if (result->length >= size - 1) break;
        );
    }
    result->c_str[size] = '\0';

    return result;
}

const string_view path_delimiter_str = const_str(PATH_DELIMITER_STR);
void concat_str_path(string* base, const string_view* path) {
    assert(base != NULL);
    assert(path != NULL);

    if (string_view_ends_with(&base->as_view, &path_delimiter_str)) {
        string_append_view(&base, path);
    } else {
        string_append_view(&base, &path_delimiter_str);
        string_append_view(&base, path);
    }
}