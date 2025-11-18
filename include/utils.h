#pragma once

#include "typedefs.h"
#include "arena.h"

struct string_chunks {
    char **items;
    usize size;
    usize capacity;
};

void split_str(struct arena *a, struct string_chunks *da, const char *input, const char *delim);
i64 parse_int(const char *source, int base);

#ifdef UTILS_IMPLEMENTATION

#include <string.h>
#include <ctype.h>
#include <errno.h>

#define ARENA_IMPLEMENTATION
#include "arena.h"

void split_str(struct arena *a, struct string_chunks *da, const char *input, const char *delim)
{
    const char *current_pos = input;
    const char *next_delim = NULL;
    size_t delim_len = strlen(delim);

    while ((next_delim = strstr(current_pos, delim)) != NULL) {
        size_t len = next_delim - current_pos;

        char *token = arena_alloc(a, len + 1);
        memcpy(token, current_pos, len);
        token[len] = '\0';
        arena_da_append(a, da, token);

        current_pos = next_delim + delim_len;
    }

    size_t last_len = strlen(current_pos);

    if (last_len > 0) {
        char *token = arena_alloc(a, last_len + 1);
        memcpy(token, current_pos, last_len);
        token[last_len] = '\0';
        arena_da_append(a, da, token);
    }
}

i64 parse_int(const char *source, int base)
{
    errno = 0;
    char *end = { 0 };
    i64 value = strtoll(source, &end, base);

    if (*end != '\0' || errno == EINVAL || errno == ERANGE) {
        fprintf(stderr, "Failed to convert \"%s\" to i64\n", source);
        exit(EXIT_FAILURE);
    }

    return value;
}

#endif // UTILS_IMPLEMENTATION
