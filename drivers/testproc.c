#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Joshin Kariyathu");


// Seting the buffer size to 1024;
#define BUFSIZE  2048
// This is the name of the proc file
#define PROCFS_N 		"buff2k"

/**
 * The buffer used to store character for this module
 * this char array can store upto BUFSIZE chars
 */
static char procfs_buffer[BUFSIZE];

/**
 * The size of the buffer
 * This is a static variable that starts at 0
 */
static unsigned long procfs_buffer_size = 0;

/**
 * This i sthe struct that holds information about the
 * /proc file
 */
static struct proc_dir_entry *proc_file;
/**
 * This function is called then the /proc file is read
 */
int procfile_read(char *buffer,
	      char **buffer_location,
	      off_t offset, int buffer_length, int *eof, void *data)
{
	static int finished = 0;
	/*
	 * return 0 to indicate emd of file.
	 * Processes will conitinue to read from file in an endless loop
	 */
	if (finished)
	{
		printk(KERN_INFO "procfs_read: END\n");
		finished = 0;
		return 0;
	}

	finished = 1;
	/*
	 *	We need to copy the string from kernel's memory segm
	 * to the memory segm of the process that called proc
	 */
	 if (copy_to_user(buffer, procfs_buffer, procfs_buffer_size))
	 {
		return -EFAULT;
	 }
	 printk(KERN_INFO "procfs_read: read %lu bytes\n", procfs_buffer_size);
	 return procfs_buffer_size; /*return the number of bytes that was read*/
}
/**
 * This function is called then the /proc file is written
 */
 int procfile_write(struct file *file, const char *buffer, size_t )
 {
	procfs_buffer_size = count;
	if (procfs_buffer_size > BUFSIZE)
	{
		procfs_buffer_size = BUFSIZE;
	}

	/*write the data to teh buffer*/
	if ( copy_from_user(procfs_buffer, buffer, procfs_buffer_size) ) {
		return -EFAULT;
	}

	return procfs_buffer_size;
}
static int module_permission(struct inode *inode, int op, struct nameidata *data)
{
	/**
	 *	Everybody has access to read from module, but
	 * only root may write to it.
	 */
	 if (op ==4 || (op == 2 && current->euid == 0))
	 	return 0;

	/* anythign else, deny access*/
	return -EACCES;
}
/*
 * The file is opened - we don't really care about
 * that, but it does mean we need to increment the
 * module's reference count.
 */
int procfs_open(struct inode *inode, struct file *file)
{
	try_module_get(THIS_MODULE);
	return 0;
}

/*
 * The file is closed - again, interesting only because
 * of the reference count.
 */
int procfs_close(struct inode *inode, struct file *file)
{
	module_put(THIS_MODULE);
	return 0;		/* success */
}
static struct file_operations myops =
{
	.read 	 = procfile_read,
	.write 	 = procfile_write,
	.open 	 = procfs_open,
	.release = procfs_close,
};
static struct inode_operations Inode_ops = {
	.permission = module_permission,	/* check for permissions */
};
/**
 *This function is called when the module is loaded
 *
 */
int init_module()
{
	/* create the /proc file */
	proc_file = create_proc_entry(PROCFS_N, 0644, NULL);

	if (proc_file == NULL) {
		remove_proc_entry(PROCFS_N, &proc_root);
		printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
			PROCFS_N);
		return -ENOMEM;
	}


	proc_file->owner	= THIS_MODULE;
	proc_file->proc_iops = &Inode_ops;
	proc_file->proc_fops = &myops;
	proc_file->mode		= S_IFREG | S_IRUGO | S_IWUSR;
	proc_file->uid 	  	= 0;
	proc_file->gid 		= 0;
	proc_file->size 	= 80;

	printk(KERN_INFO "/proc/%s created\n", PROCFS_N);
	return 0;	/* everything is ok */
}
/**
 * cleanup_module is called when the module is unloaded.
 * It removes the proc/file and prints out the message
 */
void cleanup_module()
{
	remove_proc_entry(PROCFS_N, &proc_root);
	printk(KERN_INFO "/proc/%s removed\n", PROCFS_N);
}
