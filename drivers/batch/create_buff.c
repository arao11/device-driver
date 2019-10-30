#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

/* Constants */
#define NAME "batch_buff"
#define CAPACITY 1024

static struct proc_dir_entry *syscall_proc;

static char batch_buffer[CAPACITY];

static int batch_size = 0; /* Current size of batch (in bytes) */

static int num_tasks = 0; /* Current number of tasks */


/*
 * Read buffer of sys calls
 * This should only be read when the threshold is met
 * NOTE: This is NOT handled by the proc entry
 */
int proc_read(char *buff, char **buff_location, off_t offset, int buffer_len, int *eof, void *data) {
    /* Nothing left to read */
    if (offset > 0) {
        return 0;
    }

    /* Copy data from our buffer to the buff pointer, return current size (how many bytes were read) */
    memcpy(buff, batch_buffer, batch_size);
    return batch_size;
}

/*
 * Write system call information to our batch buffer
 */
int proc_write(struct file *file, const char *buff, unsigned long count, void *data) {
    /* If we are writing over capacity, set the size appropriately */
    if (count > CAPACITY) {
        printk(KERN_ALERT "Trying to write data over capacity!\n");
        batch_size = CAPACITY;
    } else {
        batch_size = count;
    }

    /* Copy data from user space to kernel space */
    if(copy_from_user(batch_buffer, buff, batch_size) != 0) {
        return -1;
    }

    return batch_size;
}

int init_module() {
    syscall_proc = create_proc_entry(NAME, 0644, NULL);
    if (syscall_proc == NULL) {
        remote_proc_entry(NAME, &proc_root);
        printk(KERN_ALERT "ERROR: Initializing proc_dir_entry struct failed\n");
        return -ENOMEM;
    }
    
    syscall_proc->owner      = THIS_MODULE;
    syscall_proc->read_proc  = proc_read;
    syscall_proc->write_proc = proc_write;
    syscall_proc->mode       = S_IFREG | S_IRUGO;
    syscall_proc->size       = 37;
    syscall_proc->uid        = 0;
    syscall_proc->gid        = 0;

    return 0;
}

void cleanup_module() {
    remote_proc_entry(NAME, &proc_root);
}
