#include <unistd.h>

#include "file.h"
#include "mystring.h"
#include "simulation.h"

// Constants
const string_view filename = const_str("test.sim");

int main(int argc, char** argv) {
    // Print parameters
    printf("- %sParameters%s (%2d): [", ANSI_BLUE, ANSI_RESET, argc);
    for (int i = 1; i < argc; i++) {
        printf(" %s", argv[i]);
        if (i < argc - 1)
            printf(",");
    }
    printf(" ]\n");

    // Get full file path
    string* path = get_cwd_str();
    assert(path->length > 0);
    concat_str_path(path, &filename);
    printf("- %sFile path%s      : \"%s\"\n\n", ANSI_BLUE, ANSI_RESET, path->c_str);

    // Read file
    FILE* file = fopen(path->c_str, "r");
    assert(file != NULL);
    string* str = read_all_lines(file);
    fclose(file);

    simulation* sim = simulation_new(&str->as_view);
    simulation_print_fix_data(sim);
    simulation_free(sim);

    // Wait for user input
    // I put this before the free calls so I can check the memory usage
    printf("\nPress enter to exit...");
    while (getchar() != '\n');

    // Free the lines and the string
    string_free(str);
    string_free(path);

    return 0;
}