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
SYSCALL_DEFINE2(sysbuf, ReadC*, read1, int, i) {
	struct fd f = fdget(read1[i].fd);
	kernel_read(f.file, f.file->f_pos, read1[i].buff, read1[i].count);
	filp_close(f.file, NULL);
	//struct fd f = fdget(read1->fd);
	//kernel_read(f.file, f.file->f_pos, read1[i].buff, read1->count);
	//printk("COUNT:%d\n", i);
	//printk("STRUCT BUF:%s\n", (char *)read1[i].buff);
    	return 0;
}


