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
#define BUF_LEN 80
static int majorNumber;

static char kernel_buffer[256] = {0}; // zero out buffer
static char Message[BUF_LEN];
static char *Msg_ptr;
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
    /* Initializing the message  */
    Msg_ptr = Message;
    MOD_INC_USE_COUNT;
    printk(KERN_INFO "Character device %s opened successfully\n", NAME);
    return 0;
}

/**
 * Copy data from userspace into kernel buffer
 * buff: buffer that contains data from userspace. this needs to be copied to the kernel buffer
 * len: length of data in buff
 * offset:
 */
static ssize_t buffer_write(struct file *filep, const char *buff, size_t len, loff_t *offset)
{
    int j;
    for (j = 0; j < len; && j < BUF_LEN; j++)
        get_user(Message[j], buff+j);

    Msg_ptr = Message;

    return j;
}

/**
 * Copy data from buffer in kernel to *buff ptr in userspace
 */
static ssize_t buffer_read(struct file *filep, char *buff, size_t len, loff_t *offset) {

    /* Number of bytes written to the buffer */
    int bytes_read = 0;
    /* We return 0 if we are at the end of the message pointer Msg_ptr*/
    if (*Msg_ptr == 0)
    {
      return 0;
    }

    /* Now we put the data into the buffer */
    while (len && *Msg_ptr)
    {
      /**
       * put_user copies data from the kernal data segment to the user data segment.
       */
      put_user(*(Msg_ptr++), buff++);
      len--;
      bytes_read++;
    }

    return bytes_read;

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
    .ioctl = device_ioctl,
};
int device_ioctl(struct inode *inode, struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
    int i;
    char *tmp;
    char ch;

    switch (ioctl_num)
    {
        case IOCTL_SET_MSG:
          temp = (char *) ioctl_param;
          get_user(ch, tmp);
          for (i = 0; ch && i < BUF_LEN; i++, tmp++)
              get_user(cg, tmp);

          device_write(file, (char *) ioctl_param, i);
        break;

        case IOCTL_GET_MSG:
          i = device_read(file, (char *) ioctl_param, 99, 0);
          put_user('\0', (char *) ioctl_param+i);
        break;

        case IOCTL_GET_NTH_BYTE:
          return Message[ioctl_param];
        break;
    }

    return 0;
}


static int __init buffer_init(void) {
    /* Passing in 0 dynamically allocates a major number for device */
    if ((majorNumber = module_register_chrdev(100, NAME, &fops)) < 0) {
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
    module_unregister_chrdev(100, NAME);
}


module_init(buffer_init);
module_exit(buffer_exit);
