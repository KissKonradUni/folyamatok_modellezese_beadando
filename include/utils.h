#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>  // IWYU pragma: keep
#include <stdlib.h> // IWYU pragma: keep

// Assertation macros
#define assert(expr) if (!(expr)) { printf("Assertion failed: %s; at %s:%d\n", #expr, __FILE__, __LINE__); exit(1); }
#define assert_warn(expr) if (!(expr)) { printf("Warning: %s; at %s:%d\n", #expr, __FILE__, __LINE__); }

// Math macros
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define clamp(x, floor, ceil) min(max(x, floor), ceil)

// Memory macros
#define swap(a, b) { typeof(a) tmp = a; a = b; b = tmp; }

#endif // UTILS_H