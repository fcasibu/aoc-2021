#include <stdlib.h>
#define ARENA_IMPLEMENTATION
#include "arena.h"
#define FILE_IMPLEMENTATION
#include "file.h"
#define UTILS_IMPLEMENTATION
#include "utils.h"
#include "logger.h"
#include "str.h"

#define ARENA_SIZE 1024
#define FILE_NAME "day005/input.txt"

typedef struct {
    i32 x1;
    i32 y1;
    i32 x2;
    i32 y2;
} points_t;

typedef struct {
    points_t *items;
    usize size;
    usize capacity;
} vents_t;

typedef struct {
    vents_t vents;
    i32 width;
    i32 height;
} ocean_floor_t;

typedef struct {
    arena_t *arena;
    const char *source;
    ocean_floor_t *ocean_floor;
} context_t;

ocean_floor_t *parse_input(arena_t *arena, const char *source);
void solve_part1(context_t *ctx);
void solve_part2(context_t *ctx);
void fill_diagram(i32 *diagram, usize len, ocean_floor_t *ocean_floor, bool include_diag);
usize count_overlaps(const i32 *diagram, usize len);

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

    ocean_floor_t *ocean_floor = parse_input(&arena, source);

    if (!ocean_floor) {
        LOG(LOG_ERROR, "%s", "Failed to parse input");
        arena_destroy(&arena);
        return 1;
    }

    context_t ctx = {
        .arena = &arena,
        .source = source,
        .ocean_floor = ocean_floor,
    };

    solve_part1(&ctx);
    solve_part2(&ctx);

    arena_destroy(&arena);

    return 0;
}

ocean_floor_t *parse_input(arena_t *arena, const char *source)
{
    ocean_floor_t *ocean_floor = arena_alloc(arena, sizeof(*ocean_floor));
    if (!ocean_floor)
        return NULL;

    vents_t vents = { 0 };
    arena_da_init(arena, &vents, ARENA_DA_CAPACITY);

    string_chunks_t *lines = split_str(arena, source, "\n");

    for (usize i = 0; i < lines->size; ++i) {
        string_chunks_t *coords = split_str(arena, lines->items[i], " -> ");
        if (coords->size != 2)
            return NULL;

        string_chunks_t *point_one = split_str(arena, coords->items[0], ",");
        if (point_one->size != 2)
            return NULL;

        string_chunks_t *point_two = split_str(arena, coords->items[1], ",");
        if (point_one->size != 2)
            return NULL;

        i32 x1 = (i32)parse_int(point_one->items[0], 10);
        i32 y1 = (i32)parse_int(point_one->items[1], 10);
        i32 x2 = (i32)parse_int(point_two->items[0], 10);
        i32 y2 = (i32)parse_int(point_two->items[1], 10);

        ocean_floor->width = MAX(ocean_floor->width, MAX(x1, x2) + 1);
        ocean_floor->height = MAX(ocean_floor->height, MAX(y1, y2) + 1);

        points_t points = { x1, y1, x2, y2 };
        arena_da_append(arena, &vents, points);
    }

    assert(vents.size >= 0);
    ocean_floor->vents = vents;

    return ocean_floor;
}

void solve_part1(context_t *ctx)
{
    i32 diagram[ctx->ocean_floor->width * ctx->ocean_floor->height];
    usize diagram_len = sizeof(diagram) / sizeof(*diagram);
    memset(diagram, 0, sizeof(diagram));

    fill_diagram(diagram, diagram_len, ctx->ocean_floor, false);

    LOG(LOG_INFO, "Part 1 Overlaps: %zu", count_overlaps(diagram, diagram_len));
}

void solve_part2(context_t *ctx)
{
    i32 diagram[ctx->ocean_floor->width * ctx->ocean_floor->height];
    usize diagram_len = sizeof(diagram) / sizeof(*diagram);
    memset(diagram, 0, sizeof(diagram));

    fill_diagram(diagram, diagram_len, ctx->ocean_floor, true);

    LOG(LOG_INFO, "Part 2 Overlaps: %zu", count_overlaps(diagram, diagram_len));
}

void fill_diagram(i32 *diagram, usize diagram_len, ocean_floor_t *ocean_floor, bool include_diag)
{
    for (usize i = 0; i < ocean_floor->vents.size; ++i) {
        points_t points = ocean_floor->vents.items[i];

        i32 min_x = MIN(points.x1, points.x2);
        i32 max_x = MAX(points.x1, points.x2);
        i32 min_y = MIN(points.y1, points.y2);
        i32 max_y = MAX(points.y1, points.y2);

        if (min_x == max_x || min_y == max_y) {
            for (i32 y = min_y; y <= max_y; ++y) {
                for (i32 x = min_x; x <= max_x; ++x) {
                    i32 index = y * (ocean_floor->width) + x;
                    assert(index >= 0 && (usize)index < diagram_len);

                    diagram[(usize)index] += 1;
                }
            }

            continue;
        }

        if (!include_diag)
            continue;

        i32 dx = points.x2 - points.x1;
        i32 dy = points.y2 - points.y1;
        i32 steps = abs(dx);
        i32 step_x = dx > 0 ? 1 : -1;
        i32 step_y = dy > 0 ? 1 : -1;

        for (i32 j = 0; j <= steps; ++j) {
            i32 x = points.x1 + j * step_x;
            i32 y = points.y1 + j * step_y;
            i32 index = y * ocean_floor->width + x;
            assert(index >= 0 && (usize)index < diagram_len);

            diagram[(usize)index] += 1;
        }
    }
}

usize count_overlaps(const i32 *diagram, usize len)
{
    usize overlaps = 0;
    for (usize i = 0; i < len; ++i) {
        overlaps += diagram[i] >= 2;
    }

    return overlaps;
}
