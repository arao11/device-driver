#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include "buffered-sysread.h"

static pthread_t tid[READ_BUFFER_SIZE];
static pthread_mutex_t buff_lock = PTHREAD_MUTEX_INITIALIZER;
static int buff_index = 0;
static struct read_call *rcalls;

void* insert_read_request(void *arg) {
    struct read_call *call = calloc(1, sizeof(struct read_call));
    call->count = 32;
    call->buff = calloc(call->count+1, sizeof(char));
    call->fd = open("text.txt", O_RDONLY);
    
    pthread_mutex_lock(&buff_lock);
    rcalls[buff_index] = *call;
    buff_index++;
    pthread_mutex_unlock(&buff_lock);
    
    buff_insert_read(call);
    
    
    return 0;
}


int main(int ac, char *av[]) {

    rcalls = calloc(READ_BUFFER_SIZE, sizeof(struct read_call));
    
    /*int i;
    for(i = 0; i < READ_BUFFER_SIZE; i++) {
        struct read_call *call = (struct read_call *)calloc(1, sizeof(struct read_call));
        call->count = 128;
        call->buff = calloc(call->count+1, sizeof(char));
        call->path = calloc(1, PATH_MAX);
        realpath("text.txt", call->path);
        call->fd = open(call->path, O_RDONLY);
        
        //if (call->fd < 0)
        //    printf("Failed TO Open Requested File!!!\n");
        
        rcalls[i] = *call;
        buff_insert_read(call);
        free(call); // NOTE: This is needed to prevent memory leaks
    }
    
    // TODO Access read data from here
    for (i = 0; i < READ_BUFFER_SIZE; i++) {
        struct read_call call = rcalls[i];
        printf("Contents Read: %s\n\n", (char *)call.buff);
        printf("Abs Path: %s\n", call.path);
        close(call.fd);
        free(call.buff);
        free(call.path);
    }
    
    free(rcalls);
    iterations--;*/
    
    int t;
    for (t = 0; t < READ_BUFFER_SIZE; t++) {
        pthread_create(&(tid[t]), NULL, &insert_read_request, NULL);
    }
    
    for (t = 0; t < READ_BUFFER_SIZE; t++) {
        pthread_join(tid[t], NULL);
    }

    pthread_mutex_destroy(&buff_lock);
    
    for (t = 0; t < READ_BUFFER_SIZE; t++) {
        struct read_call call = rcalls[t];
        printf("Contents Read: %s\n\n", (char *)call.buff);
        close(call.fd);
        free(call.buff);
    }
    
    free(rcalls);

}
