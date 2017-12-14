#ifndef OMEM_H_
#define OMEM_H_

#include <stdio.h>

struct omem_reserved {
    char reserved[32];
};

typedef struct omem_reserved omem;

void omem_init(omem *file);
void omem_term(omem *file);
FILE *omem_open(omem *file, const char *mode);
void omem_mem(omem *file, void **mem, size_t *size);

#endif /* !OMEM_H_ */
