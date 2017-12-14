#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "omem.h"

union omem_conv {
    omem *fm;
    struct omemi_buf *buf;
};

void omem_init(omem *file)
{
    union omem_conv cv = { .fm = file };
    memset(cv.buf, 0, sizeof (*cv.buf));
}

void omem_term(omem *file)
{
    union omem_conv cv = { .fm = file };
    free(cv.buf->mem);
}

static int mem_write(void *cookie, const char *buf, int size)
{
    if (size < 0) {
        errno = EINVAL;
        return -1;
    }
    struct omem_stream *stream = cookie;

    struct omemi_buf from = { (char *) buf, size };
    struct omemi_buf to;

    if (omemi_grow(stream, size) < 0) {
        return -1;
    }
    if (omemi_cursor(&to, stream) < 0) {
        return 0;
    }

    size_t copied = omemi_copy(&to, &from);
    stream->cursor += copied;
    if (stream->buf->size < stream->cursor)
        stream->buf->size = stream->cursor;
    if (copied > INT_MAX) {
        errno = EOVERFLOW;
        return -1;
    }
    return copied;
}

static int mem_read(void *cookie, char *buf, int size)
{
    if (size < 0) {
        errno = EINVAL;
        return -1;
    }
    struct omem_stream *stream = cookie;

    struct omemi_buf to = { buf, size };
    struct omemi_buf from;

    if (omemi_cursor(&from, stream) < 0) {
        return 0;
    }

    size_t copied = omemi_copy(&to, &from);
    stream->cursor += copied;
    if (copied > INT_MAX) {
        errno = EOVERFLOW;
        return -1;
    }
    return copied;
}

static off_t mem_seek(void *cookie, off_t off, int whence)
{
    struct omem_stream *stream = cookie;

    size_t newoff;
    switch (whence) {
        case SEEK_SET: newoff = off; break;
        case SEEK_CUR: newoff = stream->cursor + off; break;
        case SEEK_END: newoff = stream->buf->size + off; break;
        default: errno = EINVAL; return -1;
    }
    if (newoff > stream->buf->size || (off_t)newoff < 0
            || newoff > (size_t)OFF_MAX) {
        errno = EOVERFLOW;
        return -1;
    }
    stream->cursor = newoff;
    return newoff;
}

static int mem_close(void *cookie)
{
    free(cookie);
    return 0;
}

FILE *omem_open(omem *file, const char *mode)
{
    union omem_conv cv = { .fm = file };

    free(cv.buf->mem);
    cv.buf->mem = malloc(128);
    if (!cv.buf->mem)
        return NULL;

    struct omem_stream *stream = malloc(sizeof (*stream));
    if (!stream) {
        free(cv.buf->mem);
        cv.buf->mem = NULL;
        return NULL;
    }

    *stream = (struct omem_stream) {
        .buf = cv.buf,
        .region_size = 128,
    };

    FILE *f = funopen(stream, mem_read, mem_write, mem_seek, mem_close);
    if (!f)
        free(stream);
    return f;
}

void omem_mem(omem *file, void **mem, size_t *size)
{
    union omem_conv cv = { .fm = file };
    *mem = cv.buf->mem;
    *size = cv.buf->size;
}
