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
        str->length = ftell(file) - (lineIndex++);
        if (str->length == size - 1)
            break;
    }
    str->c_str[size] = '\0';
    fclose(file);

    // Split the string into lines
    array(string_view)* lines = string_view_split(&str->as_view, '\n');

    // Print the lines
    string_view comment = const_str("#");

    foreach(i, lines) {
        if (lines->data[i].length == 0 ||
            string_view_starts_with(&lines->data[i], &comment)
        ) {
            continue;
        }

        uint16_t len = lines->data[i].length;
        char* c_str = lines->data[i].c_str;
        printf("%.*s\n", len, c_str);
    }

    // Free the lines and the string
    array_free(lines);
    string_free(str); 
    return 0;
}