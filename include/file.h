#pragma once

#include "arena.h"

char *get_input(struct arena *a, const char *filename);

#ifdef FILE_IMPLEMENTATION

#include <assert.h>

char *get_input(struct arena *a, const char *filename)
{
    FILE *file_ptr = fopen(filename, "r");

    if (!file_ptr) {
        perror("fopen failed");
        return NULL;
    }

    if (fseek(file_ptr, 0, SEEK_END) == -1) {
        perror("fseek failed");
        fclose(file_ptr);
        return NULL;
    }

    long size = ftell(file_ptr);

    if (size < 0) {
        perror("ftell failed");
        fclose(file_ptr);
        return NULL;
    }

    if (fseek(file_ptr, 0, SEEK_SET) == -1) {
        perror("fseek failed");
        fclose(file_ptr);
        return NULL;
    }

    assert(size > 0);
    char *input = arena_alloc(a, size + 1);
    usize bytes_read = fread(input, sizeof(char), size, file_ptr);
    input[bytes_read] = '\0';

    assert(bytes_read == (usize)size);

    if (ferror(file_ptr) != 0) {
        perror("fread failed");
        fclose(file_ptr);
        return NULL;
    }

    fclose(file_ptr);

    return input;
}

#endif // FILE_IMPLEMENTATION
