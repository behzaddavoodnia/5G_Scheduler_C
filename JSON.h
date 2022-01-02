#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
//#include "unistd.h"
#include <stdbool.h>
#include <stdint.h>
#include "config.h"

#ifndef MAX_BUF
#define MAX_BUF 256
#endif

#ifdef _MSC_VER
//#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

#pragma once

#define START_TIME start_time()
#define END_TIME_FOR(x) end_time(x)
#define END_TIME end_time_without_message()
#define COPY_STRING_FREE_SRC(dst, src) memcpy(dst,src, obj_char_count(src))


// Defining new operator to allocating mem
#define NEW(x) (x)malloc(sizeof(x)) // Function-like macro for memory allocation of size 1.
//#define NEW_SIZE(x, size_n) (x)malloc(sizeof(x)*size_n) // Function-like macro for memory allocation of size n
#define NEW_AND_INITIALIZE(x, size_n) (x)calloc(sizeof(x),size_n) // Function-like macro for memory allocation of size n and initialization
#define MIN(a, b) ((a) < (b) ? a : b) // Minimum macro
#define STRING char*

char GLOBAL_KEY_STRING[MAX_KEY_LENGTH];
char GLOBAL_VALUE_STRING[MAX_VALUE_LENGTH];
char GLOBAL_ADDRESS_STRING[MAX_BUF];
char GLOBAL_STRING[MAX_OBJECT_LENGTH];
STRING TEMP_STRING;


/* * Get total char of an object*/
int obj_char_count(const STRING char_pointer);

/* * Get an object with special key*/
STRING get_obj(const STRING key, const STRING txt);

/* * Return objects with key starting with a special substring */
STRING*get_obj_starts_with(const STRING key, const STRING txt);

/* * Counting substrings in a string */
int pattern_count(const STRING pat, const STRING txt);

/* * Get keys from a json string*/
STRING get_key(const char input_json[]);

/* * Get values from a json string*/
const STRING get_val(const char input_json[]);


/* * Read file size of specific file*/
long int find_size(const char file_name[]);


/* * Remove newline and ' ' from a string*/
STRING remove_white_spaces(char input_string[]);

/* * Convert file content to a string*/
STRING _json_to_str(STRING filedir);

/* * Remove quotation from string*/
STRING _remove_quotation(STRING input_string);

/* * Return working directory*/
STRING get_directory(STRING file_name);



