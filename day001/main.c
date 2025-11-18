#include <stdio.h>

#define ARENA_IMPLEMENTATION
#include "../include/arena.h"
#define FILE_IMPLEMENTATION
#include "../include/file.h"
#define UTILS_IMPLEMENTATION
#include "../include/utils.h"

#define ARENA_SIZE 1024

struct context {
    struct arena *a;
    struct string_chunks *chunks;
};

void solve_part2(struct context *ctx)
{
    usize measurements = 0;
    usize window_size = 3;

    for (usize i = 0; i < ctx->chunks->size - window_size; ++i) {
        i64 fourth_elem = parse_int(ctx->chunks->items[i + window_size], 10);
        i64 first_in_window = parse_int(ctx->chunks->items[i], 10);

        if (fourth_elem > first_in_window)
            measurements += 1;
    }

    printf("P2/Measurements: %zu\n", measurements);
}

void solve_part1(struct context *ctx)
{
    usize measurements = 0;
    i64 prev_measurement = -1;

    for (usize i = 0; i < ctx->chunks->size; ++i) {
        i64 current_measurement = parse_int(ctx->chunks->items[i], 10);
        if (prev_measurement != -1 && current_measurement > prev_measurement)
            measurements += 1;

        prev_measurement = current_measurement;
    }

    printf("P1/Measurements: %zu\n", measurements);
}

int main(void)
{
    struct arena a = { 0 };
    if (!arena_create(&a, ARENA_SIZE)) {
        fprintf(stderr, "Failed to create arena\n");
        return 1;
    }

    char *source = get_input(&a, "day001/input.txt");

    struct string_chunks *chunks = split_str(&a, source, "\n");

    struct context ctx = { .chunks = chunks, .a = &a };

    solve_part1(&ctx);
    solve_part2(&ctx);

    arena_destroy(&a);

    return 0;
}
