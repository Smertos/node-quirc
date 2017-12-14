#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "omem.h"

struct omem_impl {
    FILE *file;
    void *buf;
};

union omem_conv {
    omem *fm;
    struct omem_impl *impl;
};

void omem_init(omem *file)
{
    union omem_conv cv = { .fm = file };
    memset(cv.impl, 0, sizeof (*cv.impl));
}

void omem_term(omem *file)
{
    union omem_conv cv = { .fm = file };
    free(cv.impl->buf);
}

FILE *omem_open(omem *file, const char *mode)
{
    union omem_conv cv = { .fm = file };
    FILE *f = tmpfile();
    if (f) {
        free(cv.impl->buf);
        cv.impl->file = f;
    }
    return f;
}

void omem_mem(omem *file, void **mem, size_t *size)
{
    union omem_conv cv = { .fm = file };
    *mem = NULL;
    *size = 0;

    free(cv.impl->buf);
    cv.impl->buf = NULL;

    fseek(cv.impl->file, 0, SEEK_END);
    long bufsize = ftell(cv.impl->file);
    if (bufsize < 0) {
        return;
    }

    void *buf = malloc(bufsize);
    if (!buf) {
        return;
    }

    rewind(cv.impl->file);
    if (fread(buf, 1, bufsize, cv.impl->file) < (size_t)bufsize) {
        free(buf);
        return;
    }
    *mem = buf;
    *size = bufsize;
}
