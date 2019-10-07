#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");


/* Function prototypes */
static int block_init(void);
static void handle_queue_request(struct request_queue *);
static void block_exit(void);

/* Constants */
#define NAME "blockDevice"
#define PARTITIONS 1
#define SECTORS 1024 /* Disk size is ~524 KB */
#define SECTOR_SIZE 512 /* Using standard 512 byte sector size */

static int majorNumber = 0;

/* Our private internal data structure */
static struct blockDeviceStruct {
    unsigned long size;
    struct request_queue *queue;
    spinlock_t lock;
    u8 *data;
    struct gendisk *gd;
} blockDevice;

static struct block_device_operations block_fops = {
    .owner = THIS_MODULE,
    //TODO: implement ioctl
};

static void handle_queue_request(struct request_queue *queue) {
    struct request *rq;
    unsigned long startPos;
    unsigned long numberOfBytes;
    rq = blk_fetch_request(queue);
    while (rq != NULL) {
        if (rq->cmd_type != REQ_TYPE_FS) {
            printk(KERN_INFO "Skipping non FS requests\n");
            __blk_end_request_all(rq, -EIO);
            continue;
        }
        /* Starting offset position is the sector number * size of each sector */
        startPos = blk_rq_pos(rq) * SECTOR_SIZE;
        /* Number of bytes to copy starting from startPos is number of sectors * size of each sector */
        numberOfBytes = blk_rq_cur_sectors(rq) * SECTOR_SIZE;

        /* Check we have enough space */
        if ((startPos + numberOfBytes) >= blockDevice.size) {
            printk(KERN_INFO "ERROR: attempting to write/read past size %ld of disk %s\n", blockDevice.size, NAME);
            continue;
        }

        /* Handle copying data */
        switch (rq_data_dir(rq)) {
            case WRITE:
                /* memcpy data from rq buffer to device data field */
                memcpy(blockDevice.data + startPos, rq->buffer, numberOfBytes); 
                break;
            case READ:
                /* memcpy data from device data to buffer so user can read */
                memcpy(rq->buffer, blockDevice.data + startPos, numberOfBytes);
                break;
            default:
                continue;
        }

        if (! __blk_end_request_cur(rq, 0)) {
            rq = blk_fetch_request(queue);
        }
    }
}

static int __init block_init(void) { 
    /* Still use register_blkdev for dynamic majorNumber allocation & getting block device in /proc/devices */
    if ((majorNumber = register_blkdev(majorNumber, NAME)) <= 0) {
        printk(KERN_ERR "ERROR: Block device %s failed to register (%d)\n", NAME, majorNumber);
        return -majorNumber;
    }
    printk(KERN_INFO "Block device %s registered successfully with Major number: %d\n", NAME, majorNumber);

    blockDevice.size = SECTORS * SECTOR_SIZE;    
    blockDevice.gd = alloc_disk(PARTITIONS);

    if (!blockDevice.gd) {
        printk(KERN_ERR "ERROR: Allocating %s disk\n", NAME);
    }

    spin_lock_init(&blockDevice.lock);

    /* Setting main fields in gendisk struct */
    blockDevice.gd->major = majorNumber;
    blockDevice.gd->first_minor = 0;
    blockDevice.gd->fops = &block_fops;
    blockDevice.gd->private_data = &blockDevice;

    /* First we need to set up and allocate queue */
    blockDevice.queue = blk_init_queue(handle_queue_request, &blockDevice.lock);
    if (blockDevice.queue == NULL) {
        printk(KERN_ERR "ERROR: Allocating %s disk block queue\n", NAME);
    }

    /* Set queue and tell kernel what size sectors we are using for our block */
    blockDevice.queue->queuedata = &blockDevice;
    blockDevice.gd->queue = blockDevice.queue;
    blk_queue_logical_block_size(blockDevice.gd->queue, SECTOR_SIZE); 

    set_capacity(blockDevice.gd, SECTORS);

    /* Name under /proc/partitions and /sys/block */
    snprintf(blockDevice.gd->disk_name, 32, NAME); 

    /* Add disk last since this activates disk for usage */
    add_disk(blockDevice.gd);
    return 0;
}

/*
 * Cleanup module, and unregister block device/free queue
 */
static void __exit block_exit(void) {
    printk(KERN_ALERT "Exiting block device %s\n", NAME);

    /* Need to delete queue */
    if (blockDevice.gd->queue) {
        blk_cleanup_queue(blockDevice.queue);
    }
    unregister_blkdev(majorNumber, NAME);
}


module_init(block_init);
module_exit(block_exit);
