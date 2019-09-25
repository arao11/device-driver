#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

/* Function prototypes */
static int buffer_init(void);
static int buffer_open(struct inode *, struct file *);
static ssize_t buffer_write(struct file *, const char *, size_t, loff_t *);
static ssize_t buffer_read(struct file *, char *, size_t, loff_t *);
static int buffer_release(struct inode *, struct file *);
static void buffer_exit(void);

/* Constants */
#define NAME "simpleBuffer"

static int majorNumber;

static char kernel_buffer[256] = {0}; // zero out buffer

static int kernel_buffer_length = 0; // length of whats in buffer

static int numOpen; // number of devices open



static int buffer_open(struct inode *inode, struct file *filep) {
    /* Only support for one device open at a time right now */
    if (numOpen != 0) {
        printk(KERN_ALERT "Device already open\n");
        return -EAGAIN;
    }
    numOpen++;
    try_module_get(THIS_MODULE);
    printk(KERN_INFO "Character device %s opened successfully\n", NAME);
    return 0;
}

/**
 * Copy data from userspace into kernel buffer
 * buff: buffer that contains data from userspace. this needs to be copied to the kernel buffer
 * len: length of data in buff
 * offset: 
 */
static ssize_t buffer_write(struct file *filep, const char *buff, size_t len, loff_t *offset) {
    printk(KERN_INFO "Writing %s to buffer\n", buff);

    kernel_buffer_length = len;
    if ((copy_from_user(kernel_buffer, buff, kernel_buffer_length)) < 0) { 
        printk(KERN_ALERT "Copy buffer to kernel space operation failed\n");
        return -EFAULT;
    }

    printk(KERN_INFO "\"%s\" copied successfully to kernel space\n", buff);
    return kernel_buffer_length;
}

/**
 * Copy data from buffer in kernel to *buff ptr in userspace
 */
static ssize_t buffer_read(struct file *filep, char *buff, size_t len, loff_t *offset) {
    
    if (copy_to_user(buff, kernel_buffer, kernel_buffer_length) < 0) {
        printk(KERN_ALERT "Copy buffer to user space operation failed\n");
        return -EFAULT;
    }

    printk(KERN_INFO "\"%s\" copied successfully to user space\n", buff);
    return len;
}

/**
 * Release the device so another process can open it
 */
static int buffer_release(struct inode *inode, struct file *file) {
    numOpen--;
    module_put(THIS_MODULE);
    return 0;
}

static struct file_operations fops = {
    .open = buffer_open,
    .write = buffer_write,
    .read = buffer_read,
    .release = buffer_release,
};

static int __init buffer_init(void) { 
    /* Passing in 0 dynamically allocates a major number for device */
    if ((majorNumber = register_chrdev(0, NAME, &fops)) < 0) {
        printk(KERN_ERR "ERROR: Character device %s failed to register (%d)\n", NAME, majorNumber);
        return -majorNumber;
    }
    printk(KERN_INFO "Character device %s registered successfully with Major number: %d\n", NAME, majorNumber);
    return 0;
}

/*
 * Cleanup module, and unregister char device
 */
static void __exit buffer_exit(void) {
    printk(KERN_ALERT "cleaning up\n");
    unregister_chrdev(majorNumber, NAME);
}


module_init(buffer_init);
module_exit(buffer_exit);
