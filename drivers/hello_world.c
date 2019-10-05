#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("Dual BSD/GPL");

int init_module(void) {
    printk(KERN_ALERT "hi world\n");
    return 0;
}

void cleanup_module(void) {
    printk(KERN_ALERT "cleaning up\n");
}
