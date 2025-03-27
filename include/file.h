#ifndef FILE_H
#define FILE_H

#include "mystring.h"

/**
 * @brief Gets the current working directory.
 */
string* get_cwd_str();

/**
 * @brief Reads all lines from a file.
 * 
 * @param file The file to read.
 * @return string* The string containing all the lines.
 */
string* read_all_lines(FILE* file);

/**
 * @brief Tries to concatenate the given paths into the base variable.
 * @param base The base path. (Usually a folder)
 * @param path The path to concatenate to the base.
 */
void concat_str_path(string* base, const string_view* path);

#endif // FILE_H