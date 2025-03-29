#include <string.h>
#include <unistd.h>

#include "file.h"
#include "mystring.h"
#include "scheduler.h"
#include "simulation.h"

int main(int argc, char** argv) {
    // Print header
    printf("%s%s  Discrete process simulation  %s\n", ANSI_BOLD, ANSI_YELLOW, ANSI_RESET);
    printf("  by %sKonrad Soma Kiss%s, %sCNY8MP%s  \n\n", ANSI_YELLOW, ANSI_RESET, ANSI_YELLOW, ANSI_RESET);

    // Ask for input file
    string* filename = string_new(PATH_MAX);
    printf("  (%sIf not provided, %s\"./test.sim\"%s will be used%s)\n", ANSI_YELLOW, ANSI_GRAY, ANSI_YELLOW, ANSI_RESET);
    printf("- %sSimulation file%s: ", ANSI_BLUE, ANSI_RESET);
    while (true) {
        fgets(filename->c_str, filename->capacity, stdin);

        int pos = 0;
        while (filename->c_str[pos] != '\n' && filename->c_str[pos] != '\0')
            pos++;
        filename->length = pos;
        filename->c_str[pos] = '\0'; // Null-terminate the string
    
        // Check if the input is empty
        if (filename->length == 0) {
            printf("- %sWarning%s: Empty input. Treating as \"./test.sim\"\n", ANSI_YELLOW, ANSI_RESET);
            string_append_cstr(&filename, "./test.sim");
        }
        // Check if the input is a valid file
        if (filename->length > 1) {
            if (access(filename->c_str, F_OK) == 0) {
                break;
            } else {
                printf("- %sError%s: File \"%.*s\" not found. Try again: ", ANSI_RED, ANSI_RESET, (int)filename->length, filename->c_str);
            }
        }
    }
    
    // Print parameters
    printf("- %sParameters%s (%2d): [", ANSI_BLUE, ANSI_RESET, argc);
    for (int i = 1; i < argc; i++) {
        printf(" %s", argv[i]);
        if (i < argc - 1)
            printf(",");
    }
    printf(" ]\n");
    printf("- %sFile path%s      : \"%s\"\n\n", ANSI_BLUE, ANSI_RESET, filename->c_str);

    // Read file
    FILE* file = fopen(filename->c_str, "r");
    assert(file != NULL);
    string* str = read_all_lines(file);
    fclose(file);

    simulation* sim = simulation_new(&str->as_view);
    simulation_print_fix_data(sim);

    scheduler* sch = scheduler_new(sim);
    // scheduler_simulate_fcfs(sch);
    scheduler_free(sch);

    simulation_free(sim);

    // Wait for user input
    // I put this before the free calls so I can check the memory usage
    printf("\nPress enter to exit...");
    while (getchar() != '\n');

    // Free the lines and the string
    string_free(str);
    string_free(filename);

    return 0;
}