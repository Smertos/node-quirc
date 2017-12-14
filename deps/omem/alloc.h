#ifndef ALLOC_H_
#define ALLOC_H_

#include <stddef.h>

struct omemi_buf {
    char *mem;
    size_t size;
};

struct omem_stream {
    struct omemi_buf *buf;
    size_t cursor;
    size_t region_size;
};

int omemi_grow(struct omem_stream *stream, size_t required);
int omemi_cursor(struct omemi_buf *buf, struct omem_stream *from);
size_t omemi_copy(struct omemi_buf *to, struct omemi_buf *from);

#endif /* !ALLOC_H_ */
