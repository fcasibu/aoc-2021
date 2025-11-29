#define ARENA_IMPLEMENTATION
#include "arena.h"
#define FILE_IMPLEMENTATION
#include "file.h"
#define UTILS_IMPLEMENTATION
#include "utils.h"
#include "logger.h"
#include "str.h"

#define ARENA_SIZE 1024
#define FILE_NAME "day006/input.txt"
#define TIMERS_LEN 9

u64 *parse_input(arena_t *arena, const char *source);
void solve(const u64 *input, usize days);

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

    u64 *timers = parse_input(&arena, source);

    if (!timers) {
        LOG(LOG_ERROR, "%s", "Failed to parse input");
        arena_destroy(&arena);
        return 1;
    }

    solve(timers, 80);
    solve(timers, 256);

    arena_destroy(&arena);

    return 0;
}

u64 *parse_input(arena_t *arena, const char *source)
{
    u64 *timers = arena_alloc(arena, TIMERS_LEN * sizeof(*timers));
    if (!timers)
        return NULL;

    memset(timers, 0, TIMERS_LEN * sizeof(*timers));

    string_chunks_t *chunks = split_str(arena, source, ",");

    for (usize i = 0; i < chunks->size; ++i) {
        timers[(u64)parse_int(trim(chunks->items[i]), 10)] += 1;
    }

    return timers;
}

void solve(const u64 *input, usize days)
{
    u64 timers[TIMERS_LEN];
    memcpy(timers, input, sizeof(timers));

    for (usize i = 0; i < days; ++i) {
        u64 spawn = timers[0];

        timers[0] = timers[1];
        timers[1] = timers[2];
        timers[2] = timers[3];
        timers[3] = timers[4];
        timers[4] = timers[5];
        timers[5] = timers[6];
        timers[6] = timers[7] + spawn;
        timers[7] = timers[8];
        timers[8] = spawn;
    }

    usize count = 0;

    for (usize i = 0; i < TIMERS_LEN; ++i)
        count += timers[i];

    LOG(LOG_INFO, "%zu Days: %zu", days, count);
}
