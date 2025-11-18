#pragma once

#include "typedefs.h"
#include <stdbool.h>

struct arena {
    unsigned char *base;
    usize capacity;
    usize offset;
    struct arena *next;
};

bool arena_create(struct arena *a, usize capacity);
void *arena_alloc(struct arena *a, usize size);
void *arena_realloc(struct arena *a, void *ptr, usize old_size, usize size);
void arena_clean(struct arena *a);
void arena_destroy(struct arena *a);

#ifdef ARENA_IMPLEMENTATION

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define ARENA_DA_CAPACITY 256

#define arena_da_init(a, da)                                                 \
    do {                                                                     \
        (da)->capacity = ARENA_DA_CAPACITY;                                  \
        (da)->size = 0;                                                      \
        (da)->items = arena_alloc(a, (da)->capacity * sizeof(*(da)->items)); \
        if (!(da)->items) {                                                  \
            fprintf(stderr, "Out of memory\n");                              \
            exit(EXIT_FAILURE);                                              \
        }                                                                    \
    } while (0)

#define arena_da_append(a, da, item)                                                       \
    do {                                                                                   \
        if ((da)->size >= (da)->capacity) {                                                \
            (da)->capacity = (da)->capacity == 0 ? ARENA_DA_CAPACITY : (da)->capacity * 2; \
            void *tmp = arena_realloc(a, (da)->items, (da)->size * sizeof(*(da)->items),   \
                                      (da)->capacity * sizeof(*(da)->items));              \
            if (!tmp) {                                                                    \
                fprintf(stderr, "Out of memory\n");                                        \
                exit(EXIT_FAILURE);                                                        \
            }                                                                              \
            (da)->items = tmp;                                                             \
        }                                                                                  \
        (da)->items[(da)->size++] = item;                                                  \
    } while (0)

bool arena_create(struct arena *a, usize capacity)
{
    if (!a || capacity == 0)
        return false;

    a->base = malloc(capacity);
    if (!a->base)
        return false;

    a->capacity = capacity;
    a->offset = 0;
    a->next = NULL;
    return true;
}

void *arena_alloc(struct arena *a, usize size)
{
    if (!a || !a->base || size == 0)
        return NULL;

    struct arena *end = a;
    while (end->next)
        end = end->next;

    if (end->offset + size > end->capacity) {
        struct arena *next = malloc(sizeof(*next));
        if (!next)
            return NULL;

        usize new_cap = size > end->capacity * 2 ? size : end->capacity * 2;
        if (!arena_create(next, new_cap)) {
            free(next);
            return NULL;
        }

        end->next = next;
        end = next;
    }

    void *ptr = end->base + end->offset;
    end->offset += size;

    return ptr;
}

void *arena_realloc(struct arena *a, void *oldptr, usize old_size, usize new_size)
{
    assert(old_size != 0);

    if (new_size <= old_size)
        return oldptr;

    void *newptr = arena_alloc(a, new_size);
    if (!newptr)
        return NULL;

    memcpy(newptr, oldptr, old_size);
    return newptr;
}

void arena_clean(struct arena *a)
{
    while (a) {
        a->offset = 0;
        a = a->next;
    }
}

void arena_destroy(struct arena *a)
{
    if (!a)
        return;

    free(a->base);
    a->base = NULL;
    a->capacity = 0;
    a->offset = 0;

    if (a->next) {
        arena_destroy(a->next);
        free(a->next);
        a->next = NULL;
    }
}

#endif // ARENA_IMPLEMENTATION
