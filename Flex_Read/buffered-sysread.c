#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include "buffered-sysread.h"

static struct read_call * buffer;
static int buff_index = 0;

/*
 * Inserts struct read_call into buffer. Executes read
 * calls with exec_read_calls() when buffer is full.
 */
int buff_insert_read(struct read_call * call) {
    if (buffer == NULL)
        buffer = (struct read_call*)calloc(READ_BUFFER_SIZE, sizeof(struct read_call));

    buffer[buff_index] = *call;
    buff_index++;
    
    if (buff_index == READ_BUFFER_SIZE)
        exec_read_calls();
        
    return 0;
}

/*
 * Executes read calls when the buffer is full by
 * calling our syscall.
 */
static void exec_read_calls(void) {
    /*int i;
    for (i = 0; i < READ_BUFFER_SIZE; i++) {
        struct read_call call = buffer[i];
        //call.fd = open("text.txt", O_RDONLY);
        read(call.fd, call.buff, call.count);
        //close(call.fd);
        //printf("Executing Instruction %d\n", i);
        //printf("Contents Read: %s\n\n", (char *)call.buff);
    }*/
    
    // TODO Call our syscall, syscall((void *)buffer, int length) ?
    
    free(buffer);
    buffer = NULL;
    buff_index = 0;
}
