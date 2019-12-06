#ifndef __BUFFERED_SYSREAD_H
#define __BUFFERED_SYSREAD_H

#include <stddef.h>

// Size of buffer to hold read calls
#define READ_BUFFER_SIZE 512

struct read_call {
    int fd;
    char *path;
    void *buff;
    size_t count;
};

int buff_insert_read(struct read_call * call);

static void exec_read_calls(void);

#endif
