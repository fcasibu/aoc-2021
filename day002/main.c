#include <stdio.h>
#include <assert.h>

#define ARENA_IMPLEMENTATION
#include "arena.h"
#define FILE_IMPLEMENTATION
#include "file.h"
#define UTILS_IMPLEMENTATION
#include "utils.h"

#define ARENA_SIZE 1024

typedef enum {
    DIR_FORWARD,
    DIR_UP,
    DIR_DOWN,
    DIR_UNKNOWN,
} direction_t;

typedef struct {
    direction_t direction;
    i32 position;
} instruction_t;

typedef struct {
    instruction_t *items;
    usize size;
    usize capacity;
} da_instruction_t;

typedef struct {
    arena_t *arena;
    const char *source;
    da_instruction_t *instructions;
} context_t;

void solve_part1(const context_t *ctx);
void solve_part2(const context_t *ctx);
da_instruction_t *parse_input(arena_t *arena, const char *source);

int main(void)
{
    arena_t arena = { 0 };
    if (!arena_create(&arena, ARENA_SIZE)) {
        fprintf(stderr, "Failed to create arena\n");
        return 1;
    }

    char *source = get_input(&arena, "day002/input.txt");

    if (!source) {
        fprintf(stderr, "Failed to read input file\n");
        arena_destroy(&arena);
        return 1;
    }

    context_t ctx = { .instructions = parse_input(&arena, source),
                      .arena = &arena,
                      .source = source };

    solve_part1(&ctx);
    solve_part2(&ctx);

    arena_destroy(&arena);

    return 0;
}

void solve_part1(const context_t *ctx)
{
    i32 current_depth = 0;
    i32 current_horz_pos = 0;

    for (usize i = 0; i < ctx->instructions->size; ++i) {
        instruction_t instr = ctx->instructions->items[i];

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

void solve_part2(const context_t *ctx)
{
    i32 current_depth = 0;
    i32 current_horz_pos = 0;
    i32 aim = 0;

    for (usize i = 0; i < ctx->instructions->size; ++i) {
        instruction_t instr = ctx->instructions->items[i];

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

da_instruction_t *parse_input(arena_t *arena, const char *source)
{
    string_chunks_t *chunks = split_str(arena, source, "\n");
    da_instruction_t *da = arena_alloc(arena, sizeof(*da));
    arena_da_init(arena, da, ARENA_DA_CAPACITY);

    for (usize i = 0; i < chunks->size; ++i) {
        const char *chunk = chunks->items[i];
        string_chunks_t *space_chunks = split_str(arena, chunk, " ");
        assert(space_chunks->size == 2);

        direction_t dir = DIR_UNKNOWN;

        if (strcmp(space_chunks->items[0], "forward") == 0) {
            dir = DIR_FORWARD;
        } else if (strcmp(space_chunks->items[0], "up") == 0) {
            dir = DIR_UP;
        } else if (strcmp(space_chunks->items[0], "down") == 0) {
            dir = DIR_DOWN;
        }

        instruction_t instr =
            (instruction_t){ .direction = dir,
                             .position = (i32)parse_int(space_chunks->items[1], 10) };

        arena_da_append(arena, da, instr);
    }

    return da;
}
