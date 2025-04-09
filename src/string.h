#ifndef STRING_H
#define STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *data;      // Pointer to the string data
    size_t count;   // Length of the string
    size_t capacity; // Capacity of the allocated memory
} String;

// Function prototypes
String string_create_empty(size_t initial_capacity);
String string_create(const char *str);
void string_destroy(String *str);
size_t string_length(const String *str);
void string_append_string(String *str, const char *suffix);
void string_append_char(String* str, const char c);
void string_append_char_at_index(String* str, const char c, size_t index);
void string_remove_last(String *str);
void string_remove_char(String *str, size_t index);
void string_print(const String *str);

#endif // STRING_H