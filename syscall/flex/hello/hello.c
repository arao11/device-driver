#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/hashtable.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/file.h>

SYSCALL_DEFINE1(hello, int, key) {
//Key is key for hashtable. Data is char variable to store.
//Action is an int, we can do 0 for add and 1 for delete/vice versa
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
	char buf[128];
	struct fd f = fdget(key);
	kernel_read(f.file, f.file->f_pos, buf, 8);
	printk("buf:%s\n", buf);
    	return 0;
}


