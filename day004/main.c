#include <assert.h>
#include <stdio.h>

#define ARENA_IMPLEMENTATION
#include "arena.h"
#define FILE_IMPLEMENTATION
#include "file.h"
#define UTILS_IMPLEMENTATION
#include "utils.h"
#include "logger.h"

#define ARENA_SIZE 1024
#define FILE_NAME "day004/input.txt"

typedef struct {
    u32 *items;
    usize size;
    usize capacity;
} selections_t;

typedef struct {
    u32 number;
    bool marked;
} bingo_number_t;

typedef struct {
    bingo_number_t **items;
    usize size;
    usize capacity;
} bingo_card_t;

typedef struct {
    bingo_card_t *items;
    usize size;
    usize capacity;
} bingo_cards_t;

typedef struct {
    selections_t selections;
    bingo_cards_t cards;
} bingo_t;

typedef struct {
    arena_t *arena;
    const char *source;
    bingo_t *bingo;
} context_t;

bingo_t *parse_input(arena_t *arena, const char *source);
bool verify_bingo_card(bingo_card_t *card);
u32 calculate_bingo_result(bingo_card_t *winning_card, u32 last_winning_number);
void mark_bingo_card(bingo_card_t *card, u32 selection);
void solve_part1(context_t *ctx);
void solve_part2(context_t *ctx);

int main(void)
{
    arena_t arena = { 0 };

    if (!arena_create(&arena, ARENA_SIZE)) {
        LOG(LOG_ERROR, "%s", "Failed to create arena\n");
        return 1;
    }

    const char *source = get_input(&arena, FILE_NAME);

    if (!source) {
        LOG(LOG_ERROR, "Failed to read file '%s'", FILE_NAME);
        arena_destroy(&arena);
        return 1;
    }

    bingo_t *bingo = parse_input(&arena, source);

    if (!bingo) {
        LOG(LOG_ERROR, "%s", "Failed to parse input");
        arena_destroy(&arena);
        return 1;
    }

    context_t ctx = {
        .arena = &arena,
        .source = source,
        .bingo = bingo,
    };

    solve_part1(&ctx);
    solve_part2(&ctx);

    arena_destroy(&arena);

    return 0;
}

bingo_t *parse_input(arena_t *arena, const char *source)
{
    bingo_t *bingo = arena_alloc(arena, sizeof(*bingo));

    if (!bingo)
        return NULL;

    selections_t selections = { 0 };
    arena_da_init(arena, &selections);

    bingo_cards_t bingo_cards = { 0 };
    arena_da_init(arena, &bingo_cards);

    string_chunks_t *split_lines = split_str(arena, source, "\n\n");
    if (!split_lines)
        return NULL;

    string_chunks_t *selections_str = split_str(arena, split_lines->items[0], ",");
    if (!selections_str)
        return NULL;

    for (usize i = 0; i < selections_str->size; ++i) {
        arena_da_append(arena, &selections, (u32)parse_int(selections_str->items[i], 10));
    }

    for (usize i = 1; i < split_lines->size; ++i) {
        string_chunks_t *card_str = split_str(arena, split_lines->items[i], "\n");
        if (!card_str)
            return NULL;

        bingo_card_t bingo_card = { 0 };
        arena_da_init(arena, &bingo_card);

        for (usize j = 0; j < card_str->size; ++j) {
            string_chunks_t *numbers = split_str(arena, card_str->items[j], " ");
            if (!numbers)
                return NULL;

            for (usize k = 0; k < numbers->size; ++k) {
                const char *num_str = numbers->items[k];

                if (strcmp(num_str, "") == 0)
                    continue;

                bingo_number_t *bingo_number = arena_alloc(arena, sizeof(*bingo_number));
                if (!bingo_number)
                    return NULL;

                bingo_number->number = (u32)parse_int(num_str, 10);
                bingo_number->marked = false;

                arena_da_append(arena, &bingo_card, bingo_number);
            }
        }

        arena_da_append(arena, &bingo_cards, bingo_card);
    }

    assert(selections.size > 0);
    assert(bingo_cards.size > 0);

    bingo->selections = selections;
    bingo->cards = bingo_cards;

    return bingo;
}

void solve_part1(context_t *ctx)
{
    bingo_t *bingo = ctx->bingo;

    u32 last_selection = 0;
    usize winning_card = 0;
    bool done = false;

    for (usize i = 0; i < bingo->selections.size && !done; ++i) {
        u32 selection = bingo->selections.items[i];
        last_selection = selection;

        for (usize j = 0; j < bingo->cards.size && !done; ++j) {
            bingo_card_t bingo_card = bingo->cards.items[j];
            winning_card = j;

            mark_bingo_card(&bingo_card, selection);
            done = verify_bingo_card(&bingo_card);
        }
    }

    assert(winning_card >= 0 && winning_card < bingo->cards.size);

    LOG(LOG_INFO, "Part 1 Result: %d",
        calculate_bingo_result(&bingo->cards.items[winning_card], last_selection));
}

void solve_part2(context_t *ctx)
{
    bingo_t *bingo = ctx->bingo;

    u32 last_selection = 0;
    usize last_winning_card = 0;
    usize winners_count = 0;
    bool done = false;

    bool verified_cards[bingo->cards.size];
    memset(verified_cards, false, bingo->cards.size);

    for (usize i = 0; i < bingo->selections.size && !done; ++i) {
        u32 selection = bingo->selections.items[i];
        last_selection = selection;

        for (usize j = 0; j < bingo->cards.size && !done; ++j) {
            if (verified_cards[j])
                continue;

            bingo_card_t bingo_card = bingo->cards.items[j];
            last_winning_card = j;

            mark_bingo_card(&bingo_card, last_selection);

            if (verify_bingo_card(&bingo_card)) {
                verified_cards[j] = true;
                winners_count += 1;
            }

            if (winners_count == bingo->cards.size)
                done = true;
        }
    }

    assert(last_winning_card >= 0 && last_winning_card < bingo->cards.size);

    LOG(LOG_INFO, "Part 2 Result: %d",
        calculate_bingo_result(&bingo->cards.items[last_winning_card], last_selection));
}

void mark_bingo_card(bingo_card_t *card, u32 selection)
{
    for (usize k = 0; k < card->size; ++k) {
        bingo_number_t *bingo_number = card->items[k];

        if (bingo_number->marked)
            continue;

        bingo_number->marked = selection == bingo_number->number;
    }
}

u32 calculate_bingo_result(bingo_card_t *winning_card, u32 last_winning_number)
{
    u32 unmarked_values = 0;

    for (usize k = 0; k < winning_card->size; ++k) {
        bingo_number_t *bingo_number = winning_card->items[k];

        if (bingo_number->marked)
            continue;

        unmarked_values += bingo_number->number;
    }

    return unmarked_values * last_winning_number;
}

bool verify_bingo_card(bingo_card_t *card)
{
    static const usize board_size = 5;

    // rows
    for (usize i = 0; i < board_size; ++i) {
        usize count = card->items[i * board_size + 0]->marked;

        for (usize j = 1; j < board_size; ++j)
            count += card->items[i * board_size + j]->marked;

        if (count == board_size)
            return true;
    }

    // columns
    for (usize i = 0; i < board_size; ++i) {
        usize count = card->items[0 * board_size + i]->marked;

        for (usize j = 1; j < board_size; ++j)
            count += card->items[j * board_size + i]->marked;

        if (count == board_size)
            return true;
    }

    return false;
}
