#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

MODULE_LICENSE("Dual BSD/GPL");

/* Function prototypes */
int init_module(void);
void cleanup_module(void);

/* Constants */
#define NAME "simpleBuffer"

static int majorNumber;

static char kernel_buffer[256] = {0} // zero out buffer

static struct file_operations fops = {
    .read = buffer_read;
    .write = buffer_write;
};

int init_module(void) {
    printk(KERN_ALERT "Registering char device\n");
    
    if (majorNumber = register_chrdev(0, NAME, &fops) < 0) {
        print(KERN_ERR "ERROR: Character device %s failed to register (%d)\n", NAME, majorNumber);
        return majorNumber;
    }
    printk(KERN_INFO "Character device %s registered successfully with Major number: %d\n"NAME, majorNumber);
    return 0;
}

/**
 * Copy data from userspace into kernel buffer
 *
 */
static int buffer_write(struct file *filep, const char *buff, size_t len, loff_t *offset) {
    
}

static int buffer_read(struct file *filep, char *buff, size_t len, loff_t *offset) {
    printk(KERN_INFO "Device Read called\n");
    

}

void cleanup_module(void) {
    printk(KERN_ALERT "cleaning up\n");
}
