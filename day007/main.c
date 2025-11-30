#include <stdint.h>
#define ARENA_IMPLEMENTATION
#include "arena.h"
#define FILE_IMPLEMENTATION
#include "file.h"
#define UTILS_IMPLEMENTATION
#include "utils.h"
#include "logger.h"
#include "str.h"

#define ARENA_SIZE 1024
#define FILE_NAME "day007/input.txt"

typedef struct {
    u64 *items;
    usize size;
    usize capacity;
} positions_t;

typedef struct {
    positions_t positions;
    u64 min;
    u64 max;
} context_t;

context_t *parse_input(arena_t *arena, const char *source);
void solve_part1(const context_t *context);
void solve_part2(const context_t *context);
inline u64 abs_diff(u64 a, u64 b);

int main(void)
{
    arena_t arena = { 0 };

    if (!arena_create(&arena, ARENA_SIZE)) {
        LOG(LOG_ERROR, "%s", "Failed to create arena");
    }

    const char *source = get_input(&arena, FILE_NAME);

    if (!source) {
        LOG(LOG_ERROR, "Failed to read file '%s'", FILE_NAME);
        arena_destroy(&arena);
        return 1;
    }

    context_t *context = parse_input(&arena, source);

    if (!context) {
        LOG(LOG_ERROR, "%s", "Failed to parse input");
        arena_destroy(&arena);
        return 1;
    }

    solve_part1(context);
    solve_part2(context);

    arena_destroy(&arena);

    return 0;
}

context_t *parse_input(arena_t *arena, const char *source)
{
    context_t *context = arena_alloc(arena, sizeof(*context));
    if (!context)
        return NULL;

    context->max = 0;
    context->min = UINT64_MAX;

    string_chunks_t *chunks = split_str(arena, source, ",");

    positions_t positions = { 0 };
    arena_da_init(arena, &positions, chunks->size);

    for (usize i = 0; i < chunks->size; ++i) {
        u64 num = (u64)parse_int(trim(chunks->items[i]), 10);
        context->max = MAX(context->max, num);
        context->min = MIN(context->min, num);

        arena_da_append(arena, &positions, num);
    }

    assert(positions.size > 0);

    context->positions = positions;

    return context;
}

void solve_part1(const context_t *context)
{
    u64 min_fuel_count = UINT64_MAX;

    // maybe much more efficient to get the median of a sorted list
    for (u64 i = context->min; i <= context->max; ++i) {
        u64 sum = 0;
        for (usize j = 0; j < context->positions.size; ++j) {
            sum += abs_diff(context->positions.items[j], i);
        }

        min_fuel_count = MIN(min_fuel_count, sum);
    }

    LOG(LOG_INFO, "Part 1: %llu", min_fuel_count);
}

void solve_part2(const context_t *context)
{
    u64 min_fuel_count = UINT64_MAX;

    for (u64 i = context->min; i <= context->max; ++i) {
        u64 sum = 0;
        for (usize j = 0; j < context->positions.size; ++j) {
            u64 diff = abs_diff(context->positions.items[j], i);

            sum += diff * (diff + 1) / 2;
        }

        min_fuel_count = MIN(min_fuel_count, sum);
    }

    LOG(LOG_INFO, "Part 2: %llu", min_fuel_count);
}

inline u64 abs_diff(u64 a, u64 b)
{
    return a > b ? a - b : b - a;
}
