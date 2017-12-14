#define _GNU_SOURCE
#include <stdio.h>
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

FILE *omem_open(omem *file, const char *mode)
{
    (void) mode;

    union omem_conv cv = { .fm = file };
    free(cv.buf->mem);
    return open_memstream(&cv.buf->mem, &cv.buf->size);
}

void omem_mem(omem *file, void **mem, size_t *size)
{
    union omem_conv cv = { .fm = file };
    *mem = cv.buf->mem;
    *size = cv.buf->size;
}

