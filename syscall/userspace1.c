#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <fcntl.h>

#include <linux/fs.h>
#include <stdlib.h>


int main() {
    	//long int shoutout = syscall(329);
    	//printf("System call sys_hello returned %ld\n", shoutout);
    	//int fd;
	//FILE *fptr;
	//fptr = fopen("/home/bill/buf.txt", "r+");

	//if(fptr == NULL)
	//	printf("ERORRRRR");
	//else
	//	printf("HI");
	
	//int fd;
	//fd = open("/home/bill/buf.txt", O_RDWR);
	//printf("1 : fd %d\n", syscall(329,fd,"hyyyyy", 0));
	//fd = syscall(2, "/home/bill/buf.txt", O_CREAT | O_RDWR, 0600);
	int* size = {10, 10};
	char** path = {"/home/bill/buf.txt", "/home/bill/buf.txt"};
	char** buffer; 
    	printf("1 : %d\n", syscall(330,size,path, buffer));
    	return 0;
}
