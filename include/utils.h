#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h> // IWYU pragma: keep (it is used in the assert macro)

// Assertation macros
#if defined (debug) && !defined (release)
    #ifdef ASSERT_PARANOID
        #define assert(expr) \
            if (!(expr)) { fprintf(stderr, "Assertion failed: %s; at %s:%d\n", #expr, __FILE__, __LINE__); exit(1); } \
            else { printf("Assertion passed: %s; at %s:%d\n", #expr, __FILE__, __LINE__); }
        #define assert_warn(expr) \
            if (!(expr)) { printf("Warning: %s; at %s:%d\n", #expr, __FILE__, __LINE__); } \
            else { printf("Warning passed: %s; at %s:%d\n", #expr, __FILE__, __LINE__); }
    #else
        #define assert(expr) if (!(expr)) { fprintf(stderr, "Assertion failed: %s; at %s:%d\n", #expr, __FILE__, __LINE__); exit(1); }
        #define assert_warn(expr) if (!(expr)) { printf("Warning: %s; at %s:%d\n", #expr, __FILE__, __LINE__); }
    #endif
#else
    #define assert(expr)
    #define assert_warn(expr)
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
#else
    #define PATH_DELIMITER '/'
#endif

#endif // UTILS_H