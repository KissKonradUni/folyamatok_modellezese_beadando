#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>  // IWYU pragma: keep (it is used in the assert macro)
#include <stdlib.h> // IWYU pragma: keep (it is used in the assert macro)
#include <signal.h>

// Assertation macros
#if defined (debug) && !defined (release)
    #ifndef SIGTRAP
        #define SIGTRAP SIGABRT
    #endif
    #define assert(expr) if (!(expr)) { fprintf(stderr, "Assertion failed: %s; at %s:%d\n", #expr, __FILE__, __LINE__); raise(SIGTRAP); }
    #define assert_warn(expr) if (!(expr)) { printf("Warning: %s; at %s:%d\n", #expr, __FILE__, __LINE__); }
#else
    #define assert(expr) if (!(expr)) { fprintf(stderr, "Error: %s;", #expr); exit(1); }
    #define assert_warn(expr) if (!(expr)) { printf("Warning: %s;", #expr); }
#endif

// Math macros
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define clamp(x, floor, ceil) min(max(x, floor), ceil)

// Memory macros
#define swap(a, b) { typeof(a) tmp = a; a = b; b = tmp; }

// Boolean macros
#define bool uint8_t
#define true 1
#define false 0

// Platform macros
#ifdef _WIN32
    #define PATH_DELIMITER '\\'
    #define PATH_DELIMITER_STR "\\"

    // Windows has a PATH_MAX

    #define ON_WINDOWS(expression) expression
    #define ON_UNIX(expression)
#else
    #define PATH_DELIMITER '/'
    #define PATH_DELIMITER_STR "/"

    #define PATH_MAX 4096

    #define ON_WINDOWS(expression)
    #define ON_UNIX(expression) expression
#endif

// ANSI escape codes
#define ANSI_RESET   "\x1b[0m"
#define ANSI_BOLD    "\x1b[1m"
#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[94m"
#define ANSI_GRAY    "\x1b[90m"

// Input macros
#define safeInput(count, format, ...) \
    do { \
        int returned = scanf(format, __VA_ARGS__); \
        if (returned != count) { \
            printf(stderr, "\nError: Expected %d values, but got %d\n", count, returned); \
        } else { \
            break; \
        } \
    } while (1)

#endif // UTILS_H