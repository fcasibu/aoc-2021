#include <stdio.h>
#include <assert.h>

#include "../include/typedefs.h"
#define ARENA_IMPLEMENTATION
#include "../include/arena.h"
#define FILE_IMPLEMENTATION
#include "../include/file.h"
#define UTILS_IMPLEMENTATION
#include "../include/utils.h"

#define ARENA_SIZE 1024

enum direction {
    DIR_FORWARD,
    DIR_UP,
    DIR_DOWN,
    DIR_UNKNOWN,
};

struct instruction {
    enum direction direction;
    i32 position;
};

struct da_instruction {
    struct instruction *items;
    usize size;
    usize capacity;
};

struct context {
    struct arena *a;
    const char *source;
    struct da_instruction *da;
};

void solve_part1(struct context *ctx)
{
    i32 current_depth = 0;
    i32 current_horz_pos = 0;

    for (usize i = 0; i < ctx->da->size; ++i) {
        struct instruction instr = ctx->da->items[i];

        switch (instr.direction) {
        case DIR_FORWARD:
            current_horz_pos += instr.position;
            break;
        case DIR_DOWN:
            current_depth += instr.position;
            break;
        case DIR_UP:
            current_depth -= instr.position;
            break;
        default:
            fprintf(stderr, "Unknown direction\n");
            __builtin_unreachable();
        }
    }

    printf("P1/Result: %d\n", current_horz_pos * current_depth);
}

void solve_part2(struct context *ctx)
{
    i32 current_depth = 0;
    i32 current_horz_pos = 0;
    i32 aim = 0;

    for (usize i = 0; i < ctx->da->size; ++i) {
        struct instruction instr = ctx->da->items[i];

        switch (instr.direction) {
        case DIR_FORWARD:
            current_horz_pos += instr.position;
            current_depth += (aim * instr.position);
            break;
        case DIR_DOWN:
            aim += instr.position;
            break;
        case DIR_UP:
            aim -= instr.position;
            break;
        default:
            fprintf(stderr, "Unknown direction\n");
            __builtin_unreachable();
        }
    }

    printf("P2/Result: %d\n", current_horz_pos * current_depth);
}

void parse_input(struct context *ctx)
{
    struct string_chunks *chunks = split_str(ctx->a, ctx->source, "\n");
    struct da_instruction *da = arena_alloc(ctx->a, sizeof(*da));
    arena_da_init(ctx->a, da);

    for (usize i = 0; i < chunks->size; ++i) {
        const char *chunk = chunks->items[i];
        struct string_chunks *space_chunks = split_str(ctx->a, chunk, " ");
        assert(space_chunks->size == 2);

        enum direction dir = DIR_UNKNOWN;

        if (strcmp(space_chunks->items[0], "forward") == 0) {
            dir = DIR_FORWARD;
        } else if (strcmp(space_chunks->items[0], "up") == 0) {
            dir = DIR_UP;
        } else if (strcmp(space_chunks->items[0], "down") == 0) {
            dir = DIR_DOWN;
        }

        struct instruction instr =
            (struct instruction){ .direction = dir,
                                  .position = (i32)parse_int(space_chunks->items[1], 10) };

        arena_da_append(ctx->a, da, instr);
    }

    ctx->da = da;
}

struct context init_ctx(struct arena *a, const char *source)
{
    struct context ctx = { 0 };
    ctx.a = a;
    ctx.source = source;

    return ctx;
}

int main(void)
{
    struct arena a = { 0 };
    if (!arena_create(&a, ARENA_SIZE)) {
        fprintf(stderr, "Failed to create arena\n");
        return 1;
    }

    char *source = get_input(&a, "day002/input.txt");

    if (!source) {
        fprintf(stderr, "Failed to read input file\n");
        arena_destroy(&a);
        return 1;
    }

    struct context ctx = init_ctx(&a, source);
    parse_input(&ctx);

    solve_part1(&ctx);
    solve_part2(&ctx);

    arena_destroy(ctx.a);

    return 0;
}
