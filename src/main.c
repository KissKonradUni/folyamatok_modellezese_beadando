#include <unistd.h>

#include "file.h"
#include "array.h"
#include "mystring.h"

// Constants
const string_view filename = const_str("test.sim");
const string_view comment  = const_str("#");
const string_view header   = const_str("@");

int main(int argc, char** argv) {
    // Print parameters
    printf("Parameters (%d):", argc);
    for (int i = 1; i < argc; i++)
        printf(" %s", argv[i]);
    printf("\n");

    // Get full file path
    string* path = get_cwd_str();
    assert(path->length > 0);
    concat_str_path(path, &filename);
    printf("File path: %s\n", path->c_str);

    // Read file
    FILE* file = fopen(path->c_str, "r");
    assert(file != NULL);
    string* str = read_all_lines(file);
    fclose(file);

    // Split the string into lines and print them
    array(string_view)* lines = string_view_split(&str->as_view, '\n');
    foreach(i, lines) {
        if (lines->data[i].length == 0) {
            continue;
        } else if (string_view_starts_with(&lines->data[i], &comment)) {
            printf("\033[0;94m"); // Blue
        } else if (string_view_starts_with(&lines->data[i], &header)) {
            printf("\033[0;32m"); // Green
        } else {
            printf("\033[0m");    // Reset
        }

        uint16_t len = lines->data[i].length;
        char* c_str = lines->data[i].c_str;
        printf("%.*s\n", len, c_str);
    }

    // Wait for user input
    // I put this before the free calls so I can check the memory usage
    printf("\nPress enter to exit...");
    while (getchar() != '\n');

    // Free the lines and the string
    array_free(lines);
    string_free(str);
    string_free(path);

    return 0;
}