#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "buffered-sysread.h"

int main(int ac, char *av[]) {
    struct read_call * rcalls;

    int iterations = 1;
    while (iterations > 0) {
    
        rcalls = calloc(READ_BUFFER_SIZE, sizeof(struct read_call));
        
        int i;
        for(i = 0; i < READ_BUFFER_SIZE; i++) {
            struct read_call *call = (struct read_call *)calloc(1, sizeof(struct read_call));
            call->count = 128;
            call->buff = calloc(call->count+1, sizeof(char));
            call->fd = open("text.txt", O_RDONLY);
            
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
            close(call.fd);
            free(call.buff);
        }
        
        free(rcalls);
        iterations--;
    }
}
