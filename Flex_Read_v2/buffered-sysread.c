#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "buffered-sysread.h"

static struct read_call * buffer;
static int buff_index = 0;
static pthread_mutex_t buff_lock = PTHREAD_MUTEX_INITIALIZER;

/*
 * Inserts struct read_call into buffer. Executes read
 * calls with exec_read_calls() when buffer is full.
 */
int buff_insert_read(struct read_call * call) {

    pthread_mutex_lock(&buff_lock);
        
    if (buffer == NULL)
        buffer = (struct read_call*)calloc(READ_BUFFER_SIZE, sizeof(struct read_call));

    buffer[buff_index] = *call;
    buff_index++;
    printf("Job %d in Buffer\n", buff_index);
    
    if (buff_index == READ_BUFFER_SIZE)
        exec_read_calls();
    
    pthread_mutex_unlock(&buff_lock);
        
    pthread_t thread = pthread_self();
    printf("Thread Terminating: [%u]\n", thread);
    return 0;
}

/*
 * Executes read calls when the buffer is full by
 * calling our syscall.
 */
static void exec_read_calls(void) {
    int i;
    for (i = 0; i < READ_BUFFER_SIZE; i++) {
        struct read_call call = buffer[i];
        read(call.fd, call.buff, call.count);
        //printf("Executing Instruction %d\n", i);
    }
    
    // TODO Call our syscall, syscall((void *)buffer, int length) ?
    
    free(buffer);
    buffer = NULL;
    buff_index = 0;
}
