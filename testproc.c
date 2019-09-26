#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Joshin Kariyathu");


// Seting the buffer size to 1024;
#define BUFSIZE  1024
// This is the name of the proc file
#define PROCFS_N 		"buffer1k"

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
	int retn;
	printk(KERN_INFO "procfile_read (/proc/%s) called\n", PROCFS_N);

	if (offset > 0)
		return 0;

	/* Fill the buffer and then return it */
	memcpy(buffer, procfs_buffer, procfs_buffer_size);
	return procfs_buffer_size;
}
/**
 * This function is called then the /proc file is written
 */
 int procfile_write(struct file *file, const char *buffer, unsigned long count,
 		   void *data)
 {
	printk( KERN_DEBUG "write handler\n");
	return -1;
}

static struct file_operations myops =
{
	.owner = THIS_MODULE,
	.read = myread,
	.write = mywrite,
};

/**
 *This function is called when the module is loaded
 *
 */
int init_module()
{
	/* create the /proc file */
	Our_Proc_File = create_proc_entry(PROCFS_N, 0644, NULL);

	if (Our_Proc_File == NULL) {
		remove_proc_entry(PROCFS_N, &proc_root);
		printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
			PROCFS_N);
		return -ENOMEM;
	}

	Our_Proc_File->read_proc  = procfile_read;
	Our_Proc_File->write_proc = procfile_write;
	Our_Proc_File->owner 	  = THIS_MODULE;
	Our_Proc_File->mode 	  = S_IFREG | S_IRUGO;
	Our_Proc_File->uid 	  = 0;
	Our_Proc_File->gid 	  = 0;
	Our_Proc_File->size 	  = 37;

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
