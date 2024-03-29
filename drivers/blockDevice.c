#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/uaccess.h>
#include <linux/hdreg.h>

MODULE_LICENSE("Dual BSD/GPL");


/* Function prototypes */
static int block_init(void);
static void handle_queue_request(struct request_queue *);
static void block_exit(void);

/* Constants */
#define NAME "blockDevice"
#define BLK_PARTITIONS 16
#define BLK_SECTORS 1024 /* Disk size is ~524 KB */
#define BLK_SECTOR_SIZE 512 /* Using standard 512 byte sector size */

static int majorNumber = 0;

static struct request_queue *queue;

/* Our private internal data structure */
static struct blockDeviceStruct {
    unsigned long size;
    spinlock_t lock;
    u8 *data;
    struct gendisk *gd;
} blockDevice;

int block_ioctl(struct block_device *blockDev, struct hd_geometry *geo) {
	geo->start = 0;
	geo->heads = 4;
	geo->cylinders = (blockDevice.size & ~0x3f) >> 6;
	geo->sectors = 16;

	return 0;
}

static struct block_device_operations block_fops = {
    .owner = THIS_MODULE,
    .getgeo = block_ioctl
};

static void handle_queue_request(struct request_queue *queue) {
    struct request *rq;
    unsigned long startPos;
    unsigned long numberOfBytes;
    rq = blk_fetch_request(queue);
    while (rq != NULL) {
        if (rq == NULL || rq->cmd_type != REQ_TYPE_FS) {
            printk(KERN_INFO "Skipping non FS requests\n");
            __blk_end_request_all(rq, -EIO);
            continue;
        }
        /* Starting offset position is the sector number * size of each sector */
        startPos = blk_rq_pos(rq) * BLK_SECTOR_SIZE;
        /* Number of bytes to copy starting from startPos is number of sectors * size of each sector */
        numberOfBytes = blk_rq_cur_sectors(rq) * BLK_SECTOR_SIZE;

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

    blockDevice.size = BLK_SECTORS * BLK_SECTOR_SIZE;
    spin_lock_init(&blockDevice.lock);

    blockDevice.gd = alloc_disk(BLK_PARTITIONS);
    if (!blockDevice.gd) {
        printk(KERN_ERR "ERROR: Allocating %s disk\n", NAME);
        return -ENOMEM;
    }

    blockDevice.data = vmalloc(blockDevice.size);
    if (blockDevice.data == NULL) {
        printk(KERN_ERR "ERROR: Allocating data on disk\n");
        return -ENOMEM;
    }

    /* First we need to set up and allocate queue */
    queue = blk_init_queue(handle_queue_request, &blockDevice.lock);
    if (queue == NULL) {
        printk(KERN_ERR "ERROR: Allocating %s disk block queue\n", NAME);
        vfree(blockDevice.data);
        return -ENOMEM;
    }

    /* Set queue and tell kernel what size sectors we are using for our block */
    //blockDevice.queue->queuedata = &blockDevice;
    blk_queue_logical_block_size(queue, BLK_SECTOR_SIZE); 

    /* Still use register_blkdev for dynamic majorNumber allocation & getting block device in /proc/devices */
    if ((majorNumber = register_blkdev(majorNumber, NAME)) <= 0) {
        printk(KERN_ERR "ERROR: Block device %s failed to register (%d)\n", NAME, majorNumber);
        vfree(blockDevice.data);
        return -majorNumber;
    }
    printk(KERN_INFO "Block device %s registered successfully with Major number: %d\n", NAME, majorNumber);


    /* Setting main fields in gendisk struct */
    blockDevice.gd->major = majorNumber;
    blockDevice.gd->first_minor = 0;
    blockDevice.gd->fops = &block_fops;
    blockDevice.gd->private_data = &blockDevice;
    blockDevice.gd->queue = queue;

    /* Name under /proc/partitions and /sys/block */
    snprintf(blockDevice.gd->disk_name, 32,"%s\n", NAME); 

    set_capacity(blockDevice.gd, 0);
    /* Add disk last since this activates disk for usage */
    add_disk(blockDevice.gd);


    set_capacity(blockDevice.gd, BLK_SECTORS);
    return 0;
}

/*
 * Cleanup module, and unregister block device/free queue
 */
static void __exit block_exit(void) {
    printk(KERN_ALERT "Exiting block device %s\n", NAME);

    /* Need to delete queue */
    if (queue) {
        blk_cleanup_queue(queue);
    }
    
    del_gendisk(blockDevice.gd);

    unregister_blkdev(majorNumber, NAME);
    vfree(blockDevice.data);
}


module_init(block_init);
module_exit(block_exit);
