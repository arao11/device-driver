#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/hashtable.h>
#include <linux/slab.h>

#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

SYSCALL_DEFINE3(sysbuf, int*, size, char **, path, char**, buffer) {
//Key is key for hashtable. Data is char variable to store.
//Action is an int, we can do 0 for add and 1 for delete/vice versa

	//int fd;
	//mm_segment_t fs;
	int j = 0;
	char** ptr = path;
	
	while(strcmp((*ptr), "") != 0)
	{
		struct file *f;
		//char buf[size[j]];
		//int i;
		//for(i=0;i<size[j];i++)
		//	buf[i] = 0;

		printk("Count = %d, Path = %s, Size = %d\n",j, (*ptr), size[j]);

		f = filp_open(*ptr, O_RDONLY, 0);

		if (IS_ERR(f))
			printk("ERROR OPEN");
		else{
		//&f->f_pos
			kernel_read(f, f->f_pos, (*buffer), size[j]);
			printk("buf:%s\n", *buffer);
		}
		filp_close(f, NULL);
		(*ptr)++;
		(*buffer)++;
		j++;
		
	}



/*
	f = filp_open("/home/bill/buf.txt", O_RDONLY, 0);

	if (IS_ERR(f))
		printk("ERROR");
	else{
		//&f->f_pos
		kernel_read(f, f->f_pos, buf, 128);
		printk("buf:%s\n", buf);
	}
*/
/*
	else{
		fs = get_fs();
		set_fs(get_ds());
		f->f_op->read(f, buf, 128, &f->f_pos);
		set_fs(fs);
		printk(KERN_INFO "buf:%s\n", buf);

	}
	filp_close(f,NULL);
*/

/*
	int fd;
	char buf[strlen("hi12345")];
	fd = sys_open("/home/bill/buf.txt", O_CREAT | O_RDWR, 0600);
	if(fd < 0)
		printk("Open Error\n");

	if(sys_read(fd, buf, strlen("Hello!!\n")) < 0)
		printk("Read Error\n");
	else
		printk("%s\n", buf);
	sys_close(fd);
printk("Key = %d, Data = %s, Action = %d\n",key, data, action);
*/
	
	
	
    	return 0;
}


