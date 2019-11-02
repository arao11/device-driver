#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/cred.h>
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
int procfile_read(struct file *filp,	/* see include/linux/fs.h   */
			     char *buffer,	/* buffer to fill with data */
			     size_t length,	/* length of the buffer     */
			     loff_t * offset)
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
 int procfile_write(struct file *file, const char *buffer, size_t len, loff_t * off)
{
	if (len > BUFSIZE)
	{
		procfs_buffer_size = BUFSIZE;
	}
	else
	{
		procfs_buffer_size = len;
	}

	if ( copy_from_user(procfs_buffer, buffer, procfs_buffer_size) ) {
		return -EFAULT;
	}

	printk(KERN_INFO "procfs_write: write %lu bytes\n", procfs_buffer_size);

	return procfs_buffer_size;
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
	 read: procfile_read,
	 write: procfile_write,
	 open: procfs_open,
	 release: procfs_close
};
/**
 *This function is called when the module is loaded
 *
 */
int init_module()
{
	/* create the /proc file */
	proc_file = proc_create(PROCFS_N, 0, NULL, &myops);

	if (proc_file == NULL) {
		remove_proc_entry(PROCFS_N, NULL);
		printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
			PROCFS_N);
		return -ENOMEM;
	}

	printk(KERN_INFO "/proc/%s created\n", PROCFS_N);
	return 0;	/* everything is ok */
}
/**
 * cleanup_module is called when the module is unloaded.
 * It removes the proc/file and prints out the message
 */
void cleanup_module()
{
	remove_proc_entry(PROCFS_N, NULL);
	printk(KERN_INFO "/proc/%s removed\n", PROCFS_N);
}
