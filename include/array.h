#ifdef DA_IMPLEMENTATION

#include <stdlib.h>

#define DA_INIT_CAPACITY 256

#define da_init(da)                                                   \
    do {                                                              \
        var->size = 0;                                                \
        var->capacity = DA_INIT_CAPACITY;                             \
        var->items = malloc((var)->capacity * sizeof(*(var)->items)); \
        if (!var->items) {                                            \
            fprintf(stderr, "Out of memory\n");                       \
            exit(EXIT_FAILURE);                                       \
        }                                                             \
    } while (0)

#define da_append(da, item)                                                               \
    do {                                                                                  \
        if ((da)->size >= (da)->capacity) {                                               \
            (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAPACITY : (da)->capacity * 2; \
            void *tmp = realloc((da)->items, (da)->capacity * sizeof(*(da)->items));      \
            if (!tmp) {                                                                   \
                fprintf(stderr, "Out of memory\n");                                       \
                exit(EXIT_FAILURE);                                                       \
            }                                                                             \
            (da)->items = tmp;                                                            \
        }                                                                                 \
        (da)->items[(da)->size++] = item;                                                 \
    } while (0)

#define da_free(da)            \
    do {                       \
        if (da) {              \
            free((da)->items); \
            free(da);          \
            (da) = NULL;       \
        }                      \
    } while (0)

#endif // DA_IMPLEMENTATION
