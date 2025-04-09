#include "string.h"

// Functions to create a String
String string_create_empty(size_t initial_capacity)
{
    String new_string;
    new_string.count = 0;
    new_string.capacity = initial_capacity;
    new_string.data = (char *)malloc(new_string.capacity);
    
    if (new_string.data) {
        memset(new_string.data, 0, new_string.capacity);
    }
    
    return new_string;
}
String create_string(const char *str)
{
    String new_string;
    new_string.count = strlen(str);
    new_string.capacity = new_string.count + 1;
    new_string.data = (char *)malloc(new_string.capacity);
    
    if (new_string.data) {
        strcpy(new_string.data, str);
    }
    
    return new_string;
}

// Function to destroy a String
void string_destroy(String *str)
{
    free(str->data);
    str->data = NULL;
    str->count = 0;
    str->capacity = 0;
}

// Function to get the count of the String
size_t string_length(const String *str) {
    return str->count;
}

// Function to append a suffix to the String
void string_append_string(String *str, const char *suffix)
{
    size_t suffix_length = strlen(suffix);
    size_t new_length = str->count + suffix_length;
    
    // Reallocate memory if necessary
    if (new_length + 1 > str->capacity)
    {
        str->capacity = new_length + 1;
        str->data = (char *)realloc(str->data, str->capacity);
    }
    
    if (str->data)
    {
        strcat(str->data, suffix);
        str->count = new_length;
    }
}

void string_append_char(String* str, const char c)
{
    size_t new_length = str->count + 1;

    // Reallocate memory if necessary
    if (new_length + 1 > str->capacity)
    {
        str->capacity = new_length + 1;
        str->data = (char *)realloc(str->data, str->capacity);
    }
    
    if (str->data)
    {
        str->data[str->count] = c;
        str->data[new_length] = '\0';
        str->count = new_length;
    }
}

void string_append_char_at_index(String* str, const char c, size_t index)
{
    // Ensure the index is within bounds
    if (index > str->count) {
        index = str->count;
    }

    // Check if we need to expand the string's capacity
    if (str->count + 1 >= str->capacity) {
        // Double the capacity or allocate a minimum size if capacity is 0
        size_t new_capacity = (str->capacity > 0) ? str->capacity * 2 : 16;
        char* new_data = (char*)malloc(new_capacity * sizeof(char));

        if (!new_data) {
            // Handle memory allocation failure (e.g., logging, exiting)
            return;
        }

        // Copy existing data to new memory
        memcpy(new_data, str->data, str->count);

        // Free old data and update the string to point to new data
        free(str->data);
        str->data = new_data;
        str->capacity = new_capacity;
    }

    // Shift all characters to the right starting from the index
    for (size_t i = str->count; i > index; --i) {
        str->data[i] = str->data[i - 1];
    }

    // Insert the new character at the specified index
    str->data[index] = c;

    // Update the string's length
    str->count++;

    // Null-terminate the string (if necessary for your usage)
    str->data[str->count] = '\0';
}

// Function to remove the last character from the String
void string_remove_last(String *str)
{
    if (str->count > 0) {
        str->data[str->count - 1] = '\0'; // Remove last character
        str->count--;
    }
}
void string_remove_char(String *str, size_t index) {
    if (index < str->count) {
        // Shift characters to the left to remove the character at 'index'
        for (size_t i = index; i < str->count - 1; i++) {
            str->data[i] = str->data[i + 1];
        }
        str->data[str->count - 1] = '\0'; // Null-terminate the string
        str->count--;
    }
}

// Function to print the String
void string_print(const String *str) {
    printf("%s\n", str->data);
}