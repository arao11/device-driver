#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/file.h>

struct read_call {
    int fd;
    char *path;
    void *buff;
    size_t count;
};
typedef struct read_call ReadC;
SYSCALL_DEFINE2(hello, ReadC*, read1, int, size) {
	//int fd, sz;

	/*
	char *buf;
	loff_t x;

	if(key < 0)
		printk("FD error\n");

	int r;
	r = kernel_read_file_from_fd(key,&buf,&x, INT_MAX, READING_MODULE);
	printk("NUMBER IS %lld\nBUFFER IS %s\nKERNEL READ RETURNS %d\n", x, buf,r);
	*/
/*
	char buf[128];
	struct fd f = fdget(key);
	kernel_read(f.file, f.file->f_pos, buf, 8);
	printk("buf:%s\n", buf);
*/

/*
	printk("Path:%s\n", read1[0].path);
	printk("Buffer:%s\n", read1[0].buff);
	printk("Count:%zu\n", read1->count);
	printk("FD:%d\n", read1->fd);

	printk("TERSTTTT\n");
	printk("Path:%s\n", read1->path);
	printk("Buffer:%p\n", read1->buff);
	printk("Buffer:%p\n", read1[0].buff);
*/	

    int i;
    //char buf[128];
	//printk("buf:%s\n", buf);
    for (i = 0; i < size; i++) {
	struct fd f = fdget(read1[i].fd); //THIS ONE BREAKS read1[i].fd
	kernel_read(f.file, f.file->f_pos, read1[i].buff, read1[i].count);
	filp_close(f.file, NULL);
	//printk("COUNT:%d\n", i);
	//printk("STRUCT BUF:%s\n", (char *)read1[i].buff);


	//struct read_call call = read1.buff[i];
	//struct fd f = fdget(read1->fd);
	//kernel_read(f.file, f.file->f_pos, buf, 10);
	//printk("COUNT:%d\n", i);
	//printk("buf:%s\n", buf);
	//printk("STRUCT BUF:%s\n", (char *)read1[i].buff);
        //read(call.fd, call.buff, call.count);
        //printf("Executing Instruction %d\n", i);
    }

	
    	return 0;
}


