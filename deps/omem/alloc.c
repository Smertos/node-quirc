#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "alloc.h"

static inline size_t golden_growth_ceil(size_t n)
{
    /* This effectively is a return ceil(n * φ).
       φ is approximatively 207 / (2^7), so we shift our result by
       6, then perform our ceil by adding the remainder of the last division
       by 2 of the result to itself. */

    n = (n * 207) >> 6;
    n = (n >> 1) + (n & 1);
    return n;
}

int omemi_grow(struct omem_stream *stream, size_t required)
{
    if (stream->cursor > SIZE_MAX - required) {
        errno = EOVERFLOW;
        return -1;
    }
    required += stream->cursor;

    size_t newsize = stream->region_size;
    if (required <= newsize) {
        return 0;
    }

    while (required > newsize) {
        newsize = golden_growth_ceil(newsize);
    }

    char *newmem = realloc(stream->buf->mem, newsize);
    if (!newmem) {
        return -1;
    }
    stream->buf->mem = newmem;
    stream->region_size = newsize;
    return 0;
}

int omemi_cursor(struct omemi_buf *buf, struct omem_stream *from)
{
    if (from->buf->size < from->cursor) {
        return -1;
    }

    buf->mem = from->buf->mem + from->cursor;
    buf->size = from->region_size - from->cursor;
    return 0;
}

size_t omemi_copy(struct omemi_buf *to, struct omemi_buf *from)
{
    size_t copied = from->size < to->size ? from->size : to->size;
    memcpy(to->mem, from->mem, copied);
    return copied;
}
