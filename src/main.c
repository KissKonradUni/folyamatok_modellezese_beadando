#ifdef debug
    #define ASSERT_PARANOID
#endif

#include "array.h"
#include "mystring.h"

int main() {
    string* splitTest = string_new(32);
    string_append_cstr(&splitTest, "Hello, World! This is a test string.");

    array(string_view)* parts = string_split(splitTest, ' ');
    foreach (i, parts) {
        string_view* part = &(parts->data[i]);
        printf("Part %d: %.*s\n", i, part->length, part->data);
    }

    array_free(parts);
    string_free(splitTest);

    return 0;
}