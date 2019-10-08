#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
int main() {
    //long int shoutout = syscall(329);
    //printf("System call sys_hello returned %ld\n", shoutout);
    //329 is system call, 2nd parameter is hash key, third is data entry, 4th is add or 	delete 0/1
    printf("1 : %d\n", syscall(329,2,"hi", 0));
    return 0;
}
