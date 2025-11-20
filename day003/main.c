#include <_inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define ARENA_IMPLEMENTATION
#include "arena.h"
#define FILE_IMPLEMENTATION
#include "file.h"
#define UTILS_IMPLEMENTATION
#include "utils.h"

#define ARENA_SIZE 1024

typedef enum {
    RATING_OXYGEN,
    RATING_CO2,
} rating_type_t;

typedef struct {
    i32 *items;
    usize size;
    usize capacity;
} binary_chunks_t;

typedef struct {
    usize bit_count;
    binary_chunks_t *chunks;
} binary_data_t;

typedef struct {
    arena_t *arena;
    const char *source;
    binary_data_t binary_data;
} context_t;

static binary_data_t parse_input(arena_t *arena, const char *source);
static void solve_part1(const context_t *ctx);
static void solve_part2(const context_t *ctx);
static i32 calculate_rating(const context_t *ctx, rating_type_t type);

int main(void)
{
    arena_t arena = { 0 };
    if (!arena_create(&arena, ARENA_SIZE)) {
        fprintf(stderr, "Failed to create arena\n");
        return 1;
    }

    const char *source = get_input(&arena, "day003/input.txt");

    context_t ctx = {
        .arena = &arena,
        .source = source,
        .binary_data = parse_input(&arena, source),
    };

    solve_part1(&ctx);
    solve_part2(&ctx);

    arena_destroy(&arena);

    return 0;
}

static void solve_part1(const context_t *ctx)
{
    usize bit_count = ctx->binary_data.bit_count;
    i32 gamma_rate = 0;
    i32 epsilon_rate = 0;

    for (usize i = 0; i <= bit_count; ++i) {
        usize bit_idx = (bit_count - i);

        usize ones = 0;
        usize zeros = 0;

        for (usize j = 0; j < ctx->binary_data.chunks->size; ++j) {
            if ((ctx->binary_data.chunks->items[j] >> bit_idx) & 1)
                ones += 1;
            else
                zeros += 1;
        }

        gamma_rate |= (ones > zeros) << bit_idx;
        epsilon_rate |= (ones < zeros) << bit_idx;
    }

    printf("P1/Power Consumption: %d\n", gamma_rate * epsilon_rate);
}

static void solve_part2(const context_t *ctx)
{
    i32 oxygen_generator_rating = calculate_rating(ctx, RATING_OXYGEN);
    i32 co2_scrubber_rating = calculate_rating(ctx, RATING_CO2);

    printf("P2/Life Support Rating: %d\n", oxygen_generator_rating * co2_scrubber_rating);
}

static binary_data_t parse_input(arena_t *arena, const char *source)
{
    assert(source);
    assert(strlen(source) > 0);

    string_chunks_t *lines = split_str(arena, source, "\n");
    binary_chunks_t *chunks = arena_alloc(arena, sizeof(*chunks));
    arena_da_init(arena, chunks);

    for (usize i = 0; i < lines->size; ++i) {
        const char *binary_str = lines->items[i];
        const usize str_len = strlen(binary_str);
        i32 binary_val = 0;

        for (usize j = 0; j < str_len; ++j) {
            binary_val = (binary_val << 1) + (binary_str[j] - '0');
        }

        arena_da_append(arena, chunks, binary_val);
    }

    return (binary_data_t){ .chunks = chunks, .bit_count = strlen(lines->items[0]) - 1 };
}

static i32 calculate_rating(const context_t *ctx, rating_type_t type)
{
    usize size = ctx->binary_data.chunks->size;

    i32 *candidates = arena_alloc(ctx->arena, sizeof(*candidates) * size);
    memcpy(candidates, ctx->binary_data.chunks->items, sizeof(*candidates) * size);

    for (usize i = 0; i <= ctx->binary_data.bit_count && size > 1; ++i) {
        usize bit_idx = (ctx->binary_data.bit_count - i);
        usize ones = 0;
        usize zeros = 0;

        for (usize j = 0; j < size; ++j) {
            if ((candidates[j] >> bit_idx) & 1)
                ones += 1;
            else
                zeros += 1;
        }

        u8 target_bit = 0;

        switch (type) {
        case RATING_OXYGEN: {
            target_bit = ones >= zeros;
        } break;

        case RATING_CO2: {
            target_bit = ones < zeros;
        } break;
        }

        usize new_size = 0;

        for (usize j = 0; j < size; ++j) {
            u8 bit = (candidates[j] >> bit_idx) & 1;

            if (bit == target_bit) {
                i32 tmp = candidates[new_size];
                candidates[new_size] = candidates[j];
                candidates[j] = tmp;
                new_size += 1;
            }
        }

        size = new_size;
    }

    return candidates[0];
}
