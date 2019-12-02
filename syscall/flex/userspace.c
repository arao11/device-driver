#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>
int main() {
    	//long int shoutout = syscall(329);
    	//printf("System call sys_hello returned %ld\n", shoutout);
    	//printf("1 : %d\n", syscall(329,4,"hyyyyy", 0));
	//read 0, write 1, open 2
/*
	char buf[128];
	int fd;
	fd = open("/home/bill/buf.txt", O_RDWR);
	if(fd<0)
		printf("open error");
	printf("1 : %d\n", syscall(0,fd,buf, 15));
	printf("%s\n", buf);
*/	
	int fd;
	fd = open("/home/bill/buf.txt", O_RDWR);
	if(fd<0)
		printf("open error");
	printf("1 : %d\n", syscall(329,fd));
    	return 0;
}
