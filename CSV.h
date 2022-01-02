#pragma once

#include <stdio.h>
#include <string.h>
#include "JSON.h"
#pragma warning(disable : 4996)

STRING CSV_file;
char file_name[MAX_KEY_LENGTH];

void open_file(char file_to_open[]);

void add_string_to_CSV(STRING item_to_add, bool is_end_of_line);

void add_int_to_CSV(int item_to_add, bool is_end_of_line);

void add_float_to_CSV(double item_to_add, bool is_end_of_line);

void new_line_CSV();

void cluster_title();

void CSV_maker();