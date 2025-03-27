#include <unistd.h>

#include "array.h"
#include "mystring.h"

int main(int argc, char** argv) {
    // Print parameters
    printf("Parameters (%d):", argc);
    for (int i = 1; i < argc; i++) {
        printf(" %s", argv[i]);
    }
    printf("\n");

    // Get working directory
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    printf("Working directory: %s\n", cwd);

    string* file_path = string_new(128);
    string_format(&file_path, "%s%c%s", cwd, PATH_DELIMITER, "test.sim");
    printf("File path: %s\n", file_path->c_str);

    // Open file
    FILE* file = fopen(file_path->c_str, "r");
    assert(file != NULL);

    // Get the size of the file
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Create a string with the size of the file
    string* str = string_new(size);

    // Read the file into the string
    int lineIndex = 1;
    while (fgets((str->c_str) + (str->length), size - str->length, file)) {
        // The linecounting is used to emit the \0 terminators
        // that fgets adds to the end of every line.
        // This does mean that we allocate an extra byte for every line.
        str->length = ftell(file) - (lineIndex++);
    }
    str->c_str[size] = '\0';
    fclose(file);

    // Split the string into lines
    array(string_view)* lines = string_view_split(&str->as_view, '\n');

    // Print the lines that are not empty or comments
    string_view comment = const_str("#");
    string_view header  = const_str("@");

    foreach(i, lines) {
        if (lines->data[i].length == 0) {
            continue;
        } else if (string_view_starts_with(&lines->data[i], &comment)) {
            printf("\033[0;94m"); // Blue
        } else if (string_view_starts_with(&lines->data[i], &header)) {
            printf("\033[0;32m");  // Green
        } else {
            printf("\033[0m");     // Reset
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

    return 0;
}