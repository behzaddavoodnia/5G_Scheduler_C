#include "JSON.h"


STRING next_obj;

//====================== obj_char_count ======================
int obj_char_count(const STRING char_pointer) {
    return strlen(char_pointer) + 1; // Total chars of an object.
}

//====================== get_obj ======================
STRING get_obj(const STRING key,
               const STRING txt) {
    int text_pointer = 0;
    int brackets = 0;
    int size_of_obj = 0;
    STRING obj_pointer;
    bool has_key = false;
    // Traversing the text to be find in key
    while (txt[text_pointer] != '\0') // Do untill you reach the end of text
    {
        if (key[0] == txt[text_pointer]) {
            int pattern_counter = 0;
            while (key[pattern_counter] == txt[text_pointer]) {
                pattern_counter++;
                text_pointer++;
            }
            if (key[pattern_counter] == '\0' && txt[text_pointer] != '_') {
                text_pointer--;
                text_pointer -=
                        pattern_counter;
                has_key = true;
                break;
            }
        }

        text_pointer++;
    }
    if (!has_key) {
        perror("No such key!");
        fprintf(stderr, "Error opening file: %s\n", strerror(EIO));
        return "";
    }
    const STRING tmp_text_pointer = txt + text_pointer;
    size_of_obj = obj_char_count(tmp_text_pointer);
    tmp_text_pointer = txt + text_pointer;
    // Allocate memory to save object
    STRING object_string = NEW_AND_INITIALIZE(STRING, size_of_obj);
    for (
            int i = 0; i < size_of_obj; i++)
        object_string[i] = txt[i + text_pointer];
    object_string[size_of_obj - 1] = '\0';
    return object_string; // Pointer to substring contains object
}

//====================== get_obj_starts_with ======================
STRING*get_obj_starts_with(const STRING key,
                           const STRING txt) {
    STRING*return_value; // Return all found objects
    const int total_return_obj = pattern_count(key, txt);
    // Allocate memory to save object
    return_value = NEW_AND_INITIALIZE(STRING *, total_return_obj); //(char*)malloc(sizeof(char*)*(total_return_obj));
    int text_pointer = 0;
    int brackets = 0;
    int size_of_obj = 0;
    bool has_key = false;
    for (
            int value_counter = 0; value_counter < total_return_obj; value_counter++) {
        while (txt[text_pointer] != '\0') // Do untill you reach the end of text
        {
            if (key[0] == txt[text_pointer]) {
                int pattern_counter = 0;
                while (key[pattern_counter] == txt[text_pointer]) {
                    pattern_counter++;
                    text_pointer++;
                }
                if (key[pattern_counter] == '\0' && txt[text_pointer] != '_') {
                    text_pointer--;
                    text_pointer -=
                            pattern_counter;
                    has_key = true;
                    break;
                }
            }
            text_pointer++;
        }
        if (!has_key) // Key not found
        {
            fprintf(stderr, "No key %s\n", strerror(EBADF));
            return NULL;
        }
        const STRING tmp_text_pointer = txt + text_pointer;
        size_of_obj = obj_char_count(tmp_text_pointer);
        tmp_text_pointer = txt + text_pointer;
        // Allocate memory to save object
        STRING object_string = NEW_AND_INITIALIZE(STRING, size_of_obj); // (char*)malloc(sizeof(char) * size_of_obj);
        for (
                int i = 0; i < size_of_obj; i++)
            object_string[i] = txt[i + text_pointer];
        object_string[size_of_obj - 1] = '\0';
        text_pointer += (size_of_obj - 1);
        return_value[value_counter] =
                object_string;
    }
    return return_value; // Pointers to substrings that each contains an object
}

//====================== pattern_count ======================
int pattern_count(const STRING pat,
                  const STRING txt) {
    const STRING head_of_txt = txt;
    int count = 0;
    int text_pointer = 0;
    while (txt[text_pointer] != '\0') {
        if (pat[0] == txt[text_pointer]) {
            int pattern_counter = 0;
            while (pat[pattern_counter] == txt[text_pointer]) {
                pattern_counter++;
                text_pointer++;
            }
            // Count is total substrings in main string s
            if (pat[pattern_counter] == '\0' && txt[text_pointer] != '_')
                count++;
        }
        text_pointer++;
    }
    //printf("\ntotal count is: %d\n", count);
    txt = head_of_txt;
    return
            count;
}

//====================== get_key ======================
STRING get_key(const char input_json[]) {
    int string_pointer = 0;
    // A key starts with "
    while (input_json[string_pointer] != '"')
        string_pointer++;
    GLOBAL_KEY_STRING[0] = '"';
    for (
            int i = 1; i < MAX_KEY_LENGTH; i++) {
        GLOBAL_KEY_STRING[i] = input_json[string_pointer + i];

        if (GLOBAL_KEY_STRING[i] == '"') {
            GLOBAL_KEY_STRING[++i] = '\0';
            break;
        }
    }
    return GLOBAL_KEY_STRING;
}

//====================== get_value ======================
const STRING get_val(const char input_json[]) {
    int string_pointer = 0;
    int output_pointer = 0;
    // An object value starts after :
    while (input_json[string_pointer++] != ':');
    // Some objects starts with { and other starts with " or something else
    if (input_json[string_pointer] != '{') {
        while (input_json[string_pointer] != ',') {
            if (input_json[string_pointer] == '}')
                break;
            GLOBAL_VALUE_STRING[output_pointer++] = input_json[string_pointer++];
        }
        GLOBAL_VALUE_STRING[output_pointer] = '\0';
    } else {
        int brackets = 1;
        while (true) {
            GLOBAL_VALUE_STRING[output_pointer++] = input_json[string_pointer++];
            if (input_json[string_pointer] == '}')
                brackets--;
            else if (input_json[string_pointer] == '{')
                brackets++;
            if (brackets == 0) {
                GLOBAL_VALUE_STRING[output_pointer++] = '}';
                break;
            }

        }
        GLOBAL_VALUE_STRING[output_pointer] = '\0';
    }

    return GLOBAL_VALUE_STRING;
}

//====================== find_size ======================
long int find_size(const char file_name[]) {
    // Opening the file in read mode
    FILE *fp = fopen(file_name, "r");

    // Checking if the file exist or not
    if (fp == NULL) {
        fprintf(stderr, "\nError opening file: %s\n", strerror(ENOENT));
        return -1;
    }

    fseek(fp, 0L, SEEK_END);

    // Calculating the size of the file
    long int res = ftell(fp);

    // Closing the file
    fclose(fp);

    return res;
}

//====================== remove_white_spaces ======================
STRING remove_white_spaces(char input_string[]) {
    STRING tmp_str = NEW_AND_INITIALIZE(STRING, obj_char_count(input_string));
    int char_ptr = 0;
    for (int i = 0; i < JSON_STRING_SIZE; i++) {
        if (input_string[i] == ' ' || (int) input_string[i] == 10 || (int) input_string[i] == 9)
            continue;
        tmp_str[char_ptr++] = input_string[i];
        if (input_string[i] == '\0')
            break;
    }
//    for (int i = 0; i < JSON_STRING_SIZE; i++) {
//        if (input_string[i] == '\0')
//            break;
//        // 9 is tab and 10 is new line.
//        if (input_string[i] == ' ' || (int) input_string[i] == 10 || (int) input_string[i] == 9) {
//            // Skip till end of white spaces.
//            while (input_string[i] == ' ' || (int) input_string[i] == 10 || (int) input_string[i] == 9) {
//                i++;
//            }
//        }
//        input_string[char_offset] = input_string[i];
//        char_offset++;
//    }
    strcpy(input_string, tmp_str);
    input_string[char_ptr] = '\0';
    return input_string;
}

//====================== _json_to_str ======================
STRING _json_to_str(STRING filedir) {
    STRING output_str = NEW_AND_INITIALIZE(STRING, find_size(
            filedir));
    // (char*)malloc(sizeof(char) * find_size(filedir) + 10);
    char input_char;
    int string_pointer = 0;
    FILE *file = NULL;
    file = fopen(filedir, "r");
    // Read a json file and return output string.
    if (file) {
        while ((input_char = getc(file)) != EOF)
            output_str[string_pointer++] =
                    input_char;
        fclose(file);
    } else {
        //        perror("Error printed by perror");
        fprintf(stderr, "Error opening file: %s\n", strerror(ENOENT));
        //        file = ""; // ## exception handling
    }
    output_str[string_pointer] = '\0'; // Its specifies end of file
    return output_str;
}

//====================== remove_quotation ======================
STRING _remove_quotation(STRING input_string) {
    STRING tmp_str;
    tmp_str = NEW_AND_INITIALIZE(
            STRING, obj_char_count(input_string));
    //tmp_str[0] = 'J';
    int char_ptr = 0;
    for (int i = 0; i < JSON_STRING_SIZE; i++) {
        if (input_string[i] == '"')
            continue;
        tmp_str[char_ptr++] = input_string[i];
        if (input_string[i] == '\0')
            break;
    }
    return tmp_str;
}

STRING get_directory(STRING file_name) {
    getcwd(GLOBAL_ADDRESS_STRING, MAX_BUF);
    for (int i = 0; i < MAX_BUF; i++) {
        if (GLOBAL_ADDRESS_STRING[i] == '\0') {
            GLOBAL_ADDRESS_STRING[i] = '\\';
            i++;
            for (int j = 0; j < MAX_BUF; j++) {
                GLOBAL_ADDRESS_STRING[i + j] = file_name[j];
                if (file_name[j] == '\0')
                    break;
            }
            break;
        }
    }
    return GLOBAL_ADDRESS_STRING;
    //    exit(EXIT_SUCCESS);
}

