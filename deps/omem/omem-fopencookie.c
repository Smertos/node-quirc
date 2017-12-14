#define _GNU_SOURCE
#include <errno.h>
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

static ssize_t mem_write(void *cookie, const char *buf, size_t size)
{
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
    return copied;
}

static ssize_t mem_read(void *cookie, char *buf, size_t size)
{
    struct omem_stream *stream = cookie;

    struct omemi_buf to = { buf, size };
    struct omemi_buf from;

    if (omemi_cursor(&from, stream) < 0) {
        return 0;
    }

    size_t copied = omemi_copy(&to, &from);
    stream->cursor += copied;
    return copied;
}

static int mem_seek(void *cookie, off64_t *off, int whence)
{
    struct omem_stream *stream = cookie;

    size_t newoff;
    switch (whence) {
        case SEEK_SET: newoff = *off; break;
        case SEEK_CUR: newoff = stream->cursor + *off; break;
        case SEEK_END: newoff = stream->buf->size + *off; break;
        default: errno = EINVAL; return -1;
    }
    if (newoff > stream->buf->size || (off64_t)newoff < 0) {
        return -1;
    }
    *off = newoff;
    stream->cursor = newoff;
    return 0;
}

static int mem_close(void *cookie)
{
    free(cookie);
    return 0;
}

FILE *omem_open(omem *file, const char *mode)
{
    static cookie_io_functions_t funcs = {
        .read   = mem_read,
        .write  = mem_write,
        .seek   = mem_seek,
        .close  = mem_close,
    };

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

    FILE *f = fopencookie(stream, mode, funcs);
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
